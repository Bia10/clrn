#include "StdAfx.h"
#include "HostPinger.h"
#include "Host.h"


//! Host pinger implementation
//!
//! \class CHostPinger::Impl
//!
class CHostPinger::Impl
{
	//! Host information
	struct HostInfo
	{
		CHostPinger::Status::Enum_t status;		//!< host conenction status
		boost::posix_time::ptime	updated;	//!< last time status updated

		//! Ctor
		HostInfo()
			: updated(boost::posix_time::microsec_clock::local_time())
		{

		}

		HostInfo(const CHostPinger::Status::Enum_t status)
			: status(status)
			, updated(boost::posix_time::microsec_clock::local_time())
		{

		}
	};


	//! Host guid to status map type
	typedef std::map<std::string, HostInfo>	HostStatusMap;

	//! Hosts map
	typedef std::map<std::string, CHost::Ptr>		HostMap;

public:

	Impl(ILog& logger, IKernel& kernel)
		: m_Log(logger)
		, m_Kernel(kernel)
	{
		// get settings
		CProcedure script(m_Kernel);
		script.Execute(boost::format("select * from %s") % SETTINGS_TABLE_NAME, boost::bind(&Impl::SettingsCallback, this, _1));

		// subscribe to settings change event
		CEvent settings(m_Kernel, SETTINGS_TABLE_NAME);
		settings.Subscribe(boost::bind(&Impl::SettingsCallback, this, _1));

		// subscribe to hosts change events
		CEvent hostsInsert(m_Kernel, HOSTS_TABLE_NAME);
		hostsInsert.Subscribe(boost::bind(&Impl::LocalHostsCallback, this, _1));	
	}
	 
	//! Settings callback
	void SettingsCallback(const ProtoPacketPtr packet)
	{
		CHECK(packet);

		TRY 
		{
			static boost::mutex mx;
			boost::mutex::scoped_lock lock(mx);

			// set up new settings and restart work thread
			m_WorkThread.interrupt();
			m_WorkThread.join();
	
			CTable settings(*packet->mutable_job()->mutable_results(0));

			m_PingInterval = conv::cast<std::size_t>(settings["name=ping_interval"]["value"]);

			// get hosts
			CProcedure script(m_Kernel);
			CProcedure::ParamsMap params;
			script.Execute(CProcedure::Id::HostsLoad, params, boost::bind(&Impl::LocalHostsCallback, this, _1));
		}
		CATCH_PASS_EXCEPTIONS(*packet)
	}

	//! Local hosts callback
	void LocalHostsCallback(const ProtoPacketPtr packet)
	{
		CHECK(packet);

		TRY 
		{	
			boost::mutex::scoped_lock lock(m_HostStatusesMutex);

			// set up new settings and restart work thread
			m_WorkThread.interrupt();
			m_WorkThread.join();

			data::Table& data = *packet->mutable_job()->mutable_results(0);

			// received full data first time
			if (!m_pHostsTable && data::Table_Action_Insert == data.action())
			{
				// initing data from received packet
				m_pHosts = packet;
				m_pHostsTable.reset(new CTable(data));

				// local host guid
				m_LocalHostGuid = (*m_pHostsTable)["id=1"]["guid"];
				m_LocalIp		= (*m_pHostsTable)["id=1"]["ip"];
				m_LocalPort		= (*m_pHostsTable)["id=1"]["port"];

				LOG_TRACE("Inserting hosts: [%s], local host guid: [%s]") % m_pHostsTable->Size() % m_LocalHostGuid;
			}
			else
			if (data::Table_Action_Insert == data.action())
			{
				// received row on update
				m_pHostsTable->AddRows(packet->job().results(0));

				LOG_TRACE("Inserting hosts: [%s], local host guid: [%s]") % m_pHostsTable->Size() % m_LocalHostGuid;
			}
			else
			if (data::Table_Action_Delete == data.action())
			{
				LOG_TRACE("Deleting hosts: [%s], local host guid: [%s]") % m_pHostsTable->Size() % m_LocalHostGuid;

				// received rows to delete
				m_pHostsTable->EraseRows(packet->job().results(0));

				LOG_TRACE("Deleting hosts: [%s], local host guid: [%s]") % m_pHostsTable->Size() % m_LocalHostGuid;
			}

			// getting host mapping
			CProcedure script(m_Kernel);
			script.Execute(CProcedure::Id::HostMapLoad, CProcedure::ParamsMap(), boost::bind(&Impl::LocalHostMapCallback, this, _1));
		}
		CATCH_PASS_EXCEPTIONS(*packet)
	}

	//! Local host map callback
	void LocalHostMapCallback(const ProtoPacketPtr packet)
	{
		CHECK(packet);

		TRY 
		{	
			boost::mutex::scoped_lock lock(m_HostStatusesMutex);

			// initing statuses from received packet
			CTable table(*packet->mutable_job()->mutable_results(0));

			BOOST_FOREACH(const CTable::Row& row, table)
			{
				const std::string& from		= row["from"];
				const std::string& to		= row["to"];
				const std::size_t status = conv::cast<std::size_t>(row["status"]);

				if (from == m_LocalHostGuid)
					m_OutgoingHostStatuses[to] = static_cast<CHostPinger::Status::Enum_t>(status);
				else
				if (to == m_LocalHostGuid)
					m_IncomingHostStatuses[from] = static_cast<CHostPinger::Status::Enum_t>(status);
			}		

			// starting work thread
			m_WorkThread.interrupt();
			m_WorkThread.join();
			m_WorkThread = boost::thread(boost::bind(&Impl::WorkLoop, this));
		}
		CATCH_PASS_EXCEPTIONS(*packet)
	}

	//! Work thread
	void WorkLoop()
	{	
		SCOPED_LOG(m_Log);

		LOG_TRACE("CHostspinger work thread started, hosts to ping: [%s]") % (m_pHostsTable->Size() - 1);

		for (;;)
		{
			TRY 
			{
				// sleep for ping interval
				boost::this_thread::sleep(boost::posix_time::millisec(m_PingInterval));

				// check timeouts of the incoming sessions
				CheckIncomingSessions();

				// pinging each host
				BOOST_FOREACH(const CTable::Row& row, *m_pHostsTable)
				{
					// target guid
					const std::string& guid = row["guid"];

					// skipping self ping
					if (m_LocalHostGuid == guid)
						continue;

					const boost::posix_time::ptime timeNow = boost::posix_time::microsec_clock::local_time();

					IJob::TableList params;
					IJob::TablePtr param(new IJob::AutoPtr(new data::Table()));
					params.push_back(param);

					data::Table_Row* tableRow = (*param)->add_rows();
					tableRow->add_data(conv::cast<std::string>(m_PingInterval));			// ping interval
					tableRow->add_data(m_LocalIp);											// our ip
					tableRow->add_data(m_LocalPort);										// our port
					tableRow->add_data(conv::cast<std::string>(conv::ToPosix64(timeNow)));	// time when ping sended

					m_Kernel.ExecuteJob
					(
						jobs::Job_JobId_PING_HOST, 
						params, 
						row["ip"] + ":" + row["port"], 
						boost::bind
						(
							&Impl::OnPingReplyReceived, 
							this, 
							_1, 
							guid,
							timeNow
						)
					);
				}
			}
			catch (const boost::thread_interrupted&)
			{
				LOG_TRACE("CHostspinger work thread stopped.");
				break;
			}
			CATCH_IGNORE_EXCEPTIONS(m_Log, "WorkLoop failed.")
		}
	}	

	//! Remote host pinged us
	void OnRemotePingReceived(const ProtoPacketPtr packet)
	{
		CHECK(packet);

		// remote host guid
		const std::string& packetGuid = packet->from();

		// checking host mapping status
		const CHostPinger::Status::Enum_t status = CHostPinger::Status::SessionEstablished;
	
		// set up status
		CHostPinger::Status::Enum_t lastStatus = CHostPinger::Status::Unknown;
		{
			boost::mutex::scoped_lock lock(m_HostStatusesMutex);

			const HostInfo& hostInfo = m_IncomingHostStatuses[packetGuid];

			// getting last saved status
			lastStatus = hostInfo.status;

			// getting last update time
			boost::posix_time::ptime lastUpdateTime = hostInfo.updated;

			// time elapsed since last update
			const __int64 millisecSinceLastUpdate = 
				boost::posix_time::time_duration(boost::posix_time::microsec_clock::local_time() - lastUpdateTime).total_milliseconds();

			// if status SessionEstablished and time elapsed since last update > m_PingInterval * 3 - update
			if (lastStatus == status)
			{
				if (static_cast<std::size_t>(millisecSinceLastUpdate) > m_PingInterval * 3)
					;
				else
					return;
			}

			m_IncomingHostStatuses[packetGuid] = status;
		}

		// ping value
		const unsigned __int64 timeStart = conv::cast<unsigned __int64>(packet->job().params(0).rows(0).data(3));
		const boost::posix_time::ptime time = conv::FromPosix64(timeStart);

		// inserting data and sending host map change event
		InsertHostMapFromPacket(packet, packetGuid, time, true);

		// checking for outgoing session with this host, if not exists - signal host status event
		if (lastStatus != status)
		{
			boost::mutex::scoped_lock lock(m_HostStatusesMutex);
			const CHostPinger::Status::Enum_t outgoingStatus = m_OutgoingHostStatuses[packetGuid].status;

			if (CHostPinger::Status::SessionEstablished != outgoingStatus)
			{
				const ProtoPacketPtr packet(new packets::Packet());
				data::Table& resultTable = *packet->mutable_job()->add_results();
				resultTable.set_action(data::Table_Action_Insert);
				resultTable.set_id(data::Table_Id_HostStatusEvent);

				CTable table(resultTable);
				table.AddRow()["guid"] = packetGuid;

				CEvent evnt(m_Kernel, HOST_STATUS_EVENT_NAME);
				evnt.Signal(packet);
			}
		}

		// check for this host existence, if not exists add it
		BOOST_FOREACH(const CTable::Row& row, *m_pHostsTable)
		{
			const std::string& guid = row["guid"];

			if (packetGuid == guid)
				return; // already exists
		}

		const std::string ip = packet->job().params(0).rows(0).data(1);		// ping ip
		const std::string port = packet->job().params(0).rows(0).data(2);	// ping port

		// add this host
		CProcedure script(m_Kernel);
		CProcedure::ParamsMap hostParams;
		hostParams["guid"]	= packetGuid;
		hostParams["ip"]	= ip;
		hostParams["port"]	= port;
		script.Execute(CProcedure::Id::HostsCreate, hostParams, IJob::CallBackFn());
	}


	//! Ping reply received
	void OnPingReplyReceived(const ProtoPacketPtr packet, const std::string& guid, const boost::posix_time::ptime& timePingStarted)
	{
		TRY 
		{
			// detecting status
			CHostPinger::Status::Enum_t status = CHostPinger::Status::Unknown;
			if (!packet)
				status = CHostPinger::Status::Unreacheble;
			else
				status = CHostPinger::Status::SessionEstablished;
		
			// set up status
			CHostPinger::Status::Enum_t lastStatus = CHostPinger::Status::Unknown;
			{
				boost::mutex::scoped_lock lock(m_HostStatusesMutex);

				const HostInfo& hostInfo = m_OutgoingHostStatuses[guid];

				// getting last saved status
				lastStatus = hostInfo.status;

				// getting last update time
				boost::posix_time::ptime lastUpdateTime = hostInfo.updated;

				// time elapsed since last update
				const __int64 millisecSinceLastUpdate = 
					boost::posix_time::time_duration(boost::posix_time::microsec_clock::local_time() - lastUpdateTime).total_milliseconds();

				// if status SessionEstablished and time elapsed since last update > m_PingInterval * 3 - update
				if (lastStatus == status)
				{
					if (packet && static_cast<std::size_t>(millisecSinceLastUpdate) > m_PingInterval * 3)
						;
					else
						return;
				}
			}

			if (packet)
			{
				InsertHostMapFromPacket(packet, guid, timePingStarted, false);

				boost::mutex::scoped_lock lock(m_HostStatusesMutex);
				m_OutgoingHostStatuses[guid] = status;
			}
			else
			{
				// erase host map
				CProcedure script(m_Kernel);
				CProcedure::ParamsMap hostParams;
				hostParams["from"]	= m_LocalHostGuid;
				hostParams["to"]	= guid;
				script.Execute(CProcedure::Id::HostMapDelete, hostParams, IJob::CallBackFn());

				boost::mutex::scoped_lock lock(m_HostStatusesMutex);
				m_OutgoingHostStatuses[guid] = status;
			}

			if (status == lastStatus)
				return; // no need to update host status

			// checking for incoming session with this host, if not exists - signal host status event
			boost::mutex::scoped_lock lock(m_HostStatusesMutex);
			const CHostPinger::Status::Enum_t incomingStatus = m_IncomingHostStatuses[guid].status;

			if (packet && status == incomingStatus)
				return;

			if (!packet && status != incomingStatus)
				return;

			const ProtoPacketPtr eventPacket(new packets::Packet());
			data::Table& resultTable = *eventPacket->mutable_job()->add_results();
			resultTable.set_action(packet ? data::Table_Action_Insert : data::Table_Action_Delete);
			resultTable.set_id(data::Table_Id_HostStatusEvent);

			CTable table(resultTable);
			table.AddRow()["guid"] = guid;

			CEvent evnt(m_Kernel, HOST_STATUS_EVENT_NAME);
			evnt.Signal(eventPacket);
		}
		CATCH_IGNORE_EXCEPTIONS(m_Log, guid)
	}

	//! Insert new host map record from packet
	void InsertHostMapFromPacket
	(
		const ProtoPacketPtr packet, 
		const std::string& guid, 
		const boost::posix_time::ptime& timePingStarted,
		const bool incoming
	)
	{
		CHECK(packet);

		TRY 
		{
			// getting packet endpoint
			const std::string& ep = packet->ep();
	
			// delimiter
			const std::string::size_type delimiter = ep.find(':');
	
			CHECK(std::string::npos != delimiter, ep);
	
			std::string ip;
			std::string port;
			if (incoming)
			{
				ip = ep.substr(0, delimiter);
				port = ep.substr(delimiter + 1);
			}
			else
			{
				// getting host ip and port by guid
				const CTable::Row& row = (*m_pHostsTable)[(boost::format("guid=%s") % guid).str()];
				ip = row["ip"];
				port = row["port"];
			}
	
			// getting ping interval
			const boost::posix_time::time_duration td(boost::posix_time::microsec_clock::local_time() - timePingStarted);
			__int64 pingMillisec = td.total_milliseconds();
			if (pingMillisec < 0)
				pingMillisec = 0;
	
			const std::string ping = conv::cast<std::string>(pingMillisec);

			// insert host map
			CProcedure script(m_Kernel);
			CProcedure::ParamsMap mapParams;

			mapParams["guid"]		= guid;
			mapParams["incoming"]	= conv::cast<std::string>(incoming);
			mapParams["status"]		= conv::cast<std::string>(Status::SessionEstablished);
			mapParams["ping"]		= ping;
			mapParams["ip"]			= ip;
			mapParams["port"]		= port;	

			script.Execute(CProcedure::Id::HostMapCreate, mapParams, IJob::CallBackFn());	
		}
		CATCH_PASS_EXCEPTIONS(*packet, guid)
	}

	//! Check incoming sessions
	void CheckIncomingSessions()
	{
		TRY 
		{
			boost::mutex::scoped_lock lock(m_HostStatusesMutex);

			// current time
			const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

			std::vector<std::string> timedOut;
			BOOST_FOREACH(const HostStatusMap::value_type& host, m_IncomingHostStatuses)
			{
				// time duration of the current status
				const boost::posix_time::time_duration td(now - host.second.updated);

				const __int64 millisecondSinceLastUpdate = td.total_milliseconds();

				if (millisecondSinceLastUpdate > m_PingInterval * 5)
				{
					// incoming host session timed out
					timedOut.push_back(host.first);
				}
			}

			BOOST_FOREACH(const std::string& guid, timedOut)
			{
				m_IncomingHostStatuses.erase(guid);

				// erasing host map data
				CProcedure proc(m_Kernel);
				CProcedure::ParamsMap params;
				params["from"]	= guid;
				params["to"]	= m_LocalHostGuid;
				proc.Execute(CProcedure::Id::HostMapDelete, params, IJob::CallBackFn());

				// checking for outgoing session with this host, if not exists - signal host status event
				if (CHostPinger::Status::SessionEstablished != m_OutgoingHostStatuses[guid].status)
				{
					const ProtoPacketPtr packet(new packets::Packet());
					data::Table& resultTable = *packet->mutable_job()->add_results();
					resultTable.set_action(data::Table_Action_Delete);
					resultTable.set_id(data::Table_Id_HostStatusEvent);

					CTable table(resultTable);
					table.AddRow()["guid"] = guid;

					CEvent evnt(m_Kernel, HOST_STATUS_EVENT_NAME);
					evnt.Signal(packet);
				}
			}
		}
		CATCH_IGNORE_EXCEPTIONS(m_Log)
	}

private:

	//! Logger
	ILog&						m_Log;

	//! Kernel
	IKernel&					m_Kernel;

	//! Work thread 
	boost::thread				m_WorkThread;

	//! Local host guid
	std::string					m_LocalHostGuid;

	//! Local ip
	std::string					m_LocalIp;

	//! Local port
	std::string					m_LocalPort;

	//! Ping interval
	std::size_t					m_PingInterval;

	//! Local hosts data
	ProtoPacketPtr				m_pHosts;

	//! Local hosts table wrapper
	CTable::Ptr					m_pHostsTable;

	//! Incoming host indexes map
	HostStatusMap				m_IncomingHostStatuses;

	//! Outgoing host indexes map
	HostStatusMap				m_OutgoingHostStatuses;

	//! Host statuses mutex
	boost::mutex				m_HostStatusesMutex;

	//! Host map mutex
	boost::mutex				m_HostMapMutex;

	//! Host map
	HostMap						m_HostMap;

};


boost::scoped_ptr<CHostPinger> CHostPinger::s_pInstance;

CHostPinger::CHostPinger(ILog& logger, IKernel& kernel)
	: m_pImpl(new Impl(logger, kernel))
{

}

CHostPinger::~CHostPinger(void)
{
}

CHostPinger& CHostPinger::Instance()
{
	return *s_pInstance;
}

void CHostPinger::Create(ILog& logger, IKernel& kernel)
{
	s_pInstance.reset(new CHostPinger(logger, kernel));
}

void CHostPinger::Shutdown()
{
	s_pInstance.reset();
}

void CHostPinger::OnRemotePingReceived(const ProtoPacketPtr packet)
{
	m_pImpl->OnRemotePingReceived(packet);
}
