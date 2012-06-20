#include "StdAfx.h"
#include "PingHost.h"

//! Host implementation
class CPingHost::Impl : boost::noncopyable
{
public:

	Impl
	(
		IKernel& kernel, 
		ILog& log, 
		const std::string& localGuid,
		const std::string& remoteGuid,
		const std::string& localIp,
		const std::string& localPort,
		const std::size_t pingInterval
	)
		: m_Kernel(kernel)
		, m_Log(log)
		, m_LocalHostGuid(localGuid)
		, m_RemoteHostGuid(remoteGuid)
		, m_LocalIp(localIp)
		, m_LocalPort(localPort)
		, m_PingInterval(pingInterval)
		, m_IncomingStatus(Status::Unknown)
		, m_OutgoingStatus(Status::Unknown)
	{
		SignalStatusEvent(Status::Unreacheble);
	}

	//! Set direct ep
	void DirectEndpoint(const std::string& ip, const std::string& port)
	{
		boost::mutex::scoped_lock lock(m_DataMutex);
		m_DirectEndpoint = ip + ":" + port;
	}

	//! Set NAT ep
	void NATEndpoint(const std::string& ip, const std::string& port)
	{
		boost::mutex::scoped_lock lock(m_DataMutex);
		m_NATEndpoint = ip + ":" + port;

		// nat endpoint equal to direct, ignore it
		if (m_NATEndpoint == m_DirectEndpoint)
			m_NATEndpoint.clear();
	}

	//! Ping interval
	void PingInterval(const std::size_t interval)
	{
		m_PingInterval = interval;
	}

	//! Set incoming status
	void IncomingStatus(const Status::Enum_t status)
	{
		m_IncomingStatus = status;
		SignalStatusEvent(status);
	}

	//! Set outgoing status
	void OutgoingStatus(const Status::Enum_t status)
	{
		m_OutgoingStatus = status;
		SignalStatusEvent(status);
	}

	//! Ping endpoint
	void Ping(const std::string& ep)
	{
		TRY 
		{
			// to avoid self ping(hosts endpoint may be equal on different PC)
			if (ep == m_LocalIp + ":" + m_LocalPort)
				return;

			const boost::posix_time::ptime timeNow = boost::posix_time::microsec_clock::local_time();
	
			IJob::TableList params;
			IJob::TablePtr param(new IJob::AutoPtr(new data::Table()));
			params.push_back(param);
	
			data::Table_Row* tableRow = (*param)->add_rows();
			tableRow->add_data(conv::cast<std::string>(m_PingInterval));			// ping interval
			tableRow->add_data(ep);													// endpoint
			tableRow->add_data(m_LocalIp);											// our ip
			tableRow->add_data(m_LocalPort);										// our port
			tableRow->add_data(conv::cast<std::string>(conv::ToPosix64(timeNow)));	// time when ping sended
	
			m_Kernel.ExecuteJob
			(
				jobs::Job_JobId_PING_HOST, 
				params, 
				m_RemoteHostGuid, 
				boost::bind
				(
					&Impl::PingCallBack, 
					this, 
					_1, 
					timeNow,
					ep
				)
			);
		}
		CATCH_PASS_EXCEPTIONS(ep)
	}

	//! Ping host endpoints
	void Ping()
	{
		boost::mutex::scoped_lock lock(m_DataMutex);

		TRY 
		{
			// pinging existed EPs
			if (!m_DirectEndpoint.empty())
			{
				Ping(m_DirectEndpoint);
			}
	
			if (!m_NATEndpoint.empty())
			{
				Ping(m_NATEndpoint);
			}
		}
		CATCH_IGNORE_EXCEPTIONS(m_Log)

		// checking incoming session
		if (Status::SessionEstablished != m_IncomingStatus)
			return; // nothing to check

		// current time
		const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

		// time duration of the current status
		const boost::posix_time::time_duration td(now - m_IncomingLastUpdateTime);

		const __int64 millisecondSinceLastUpdate = td.total_milliseconds();

		if (millisecondSinceLastUpdate > m_PingInterval * TIMEOUT_PING_RATIO)
		{
			// incoming host session timed out

			// erasing host map data
			CProcedure proc(m_Kernel);
			CProcedure::ParamsMap params;
			params["from"]	= m_RemoteHostGuid;
			params["to"]	= m_LocalHostGuid;
			proc.Execute(CProcedure::Id::HostMapDelete, params, IJob::CallBackFn());

			m_IncomingLastUpdateTime = boost::posix_time::microsec_clock::local_time();
			m_IncomingStatus = Status::Unreacheble;

			// signal status event
			SignalStatusEvent(Status::Unreacheble);
		}
	}

	//! Insert new host map record from packet
	void InsertHostMapRecord
	(
		const boost::posix_time::ptime& timePingStarted,
		const bool incoming,
		const std::string& ep
	)
	{
		TRY 
		{
			// delimiter
			const std::string::size_type delimiter = ep.find(':');
	
			CHECK(std::string::npos != delimiter, ep);
	
			const std::string ip = ep.substr(0, delimiter);
			const std::string port = ep.substr(delimiter + 1);
	
			// getting ping interval
			const boost::posix_time::time_duration td(boost::posix_time::microsec_clock::local_time() - timePingStarted);
			__int64 pingMillisec = td.total_milliseconds();
			if (pingMillisec < 0)
				pingMillisec = 0;
	
			const std::string ping = conv::cast<std::string>(pingMillisec);

			// insert host map
			CProcedure script(m_Kernel);
			CProcedure::ParamsMap mapParams;

			if (incoming)
			{
				mapParams["from"]	= m_RemoteHostGuid;
				mapParams["to"]		= m_LocalHostGuid;
			}
			else
			{
				mapParams["from"]	= m_LocalHostGuid;
				mapParams["to"]		= m_RemoteHostGuid;
			}
			mapParams["status"]		= conv::cast<std::string>(Status::SessionEstablished);
			mapParams["ping"]		= ping;
			mapParams["ip"]			= ip;
			mapParams["port"]		= port;	

			script.Execute(CProcedure::Id::HostMapCreate, mapParams, IJob::CallBackFn());	
		}
		CATCH_PASS_EXCEPTIONS()
	}

	//! Ping callback
	void PingCallBack(const ProtoPacketPtr packet, const boost::posix_time::ptime& timePingStarted, const std::string& endpoint)
	{
		boost::mutex::scoped_lock lock(m_DataMutex);

		const Status::Enum_t status = packet ? Status::SessionEstablished : Status::Unreacheble;

		if (status == m_OutgoingStatus)
		{
			if (Status::Unreacheble == status)
				return; // nothing to update

			// detecting time elapsed since last update
			const __int64 millisecSinceLastUpdate = 
				boost::posix_time::time_duration(boost::posix_time::microsec_clock::local_time() - m_OutgoingLastUpdateTime).total_milliseconds();

			if (millisecSinceLastUpdate < m_PingInterval * HOST_MAP_REFRESH_RATIO)
				return;
		}

		// ready to update status
		if (Status::Unreacheble == status)
		{
			// erase host map
			CProcedure script(m_Kernel);
			CProcedure::ParamsMap hostParams;
			hostParams["from"]	= m_LocalHostGuid;
			hostParams["to"]	= m_RemoteHostGuid;
			script.Execute(CProcedure::Id::HostMapDelete, hostParams, IJob::CallBackFn());
		}
		else
		if (Status::SessionEstablished == status)
		{
			InsertHostMapRecord(timePingStarted, false, endpoint);
		}

		m_OutgoingLastUpdateTime = boost::posix_time::microsec_clock::local_time();
		m_OutgoingStatus = status;

		SignalStatusEvent(status);
	}

	//! Signal status event
	void SignalStatusEvent(const Status::Enum_t status)
	{
		// if statuses are equal signal event
		if (Status::SessionEstablished == status && m_IncomingStatus != m_OutgoingStatus)
			;
		else
		if (Status::SessionEstablished != status && m_IncomingStatus == m_OutgoingStatus)
			;
		else
			return;
		
		const ProtoPacketPtr packet(new packets::Packet());
		data::Table& resultTable = *packet->mutable_job()->add_results();
		resultTable.set_action(Status::SessionEstablished == status ? data::Table_Action_Insert : data::Table_Action_Delete);
		resultTable.set_id(data::Table_Id_HostStatusEvent);

		CTable table(resultTable);
		table.AddRow()["guid"] = m_RemoteHostGuid;

		CEvent evnt(m_Kernel, HOST_STATUS_EVENT_NAME);
		evnt.Signal(packet);
	}

	//! Handle ping request packet
	void HandleRequest(const ProtoPacketPtr packet)
	{
		boost::mutex::scoped_lock lock(m_DataMutex);

		CHECK(packet);

		const Status::Enum_t status = Status::SessionEstablished;

		if (status == m_IncomingStatus)
		{
			// detecting time elapsed since last update
			const __int64 millisecSinceLastUpdate = 
				boost::posix_time::time_duration(boost::posix_time::microsec_clock::local_time() - m_IncomingLastUpdateTime).total_milliseconds();

			if (millisecSinceLastUpdate < m_PingInterval * HOST_MAP_REFRESH_RATIO)
				return;
		}

		// ping value
		const unsigned __int64 timeStart = conv::cast<unsigned __int64>(packet->job().params(0).rows(0).data(4));
		const boost::posix_time::ptime time = conv::FromPosix64(timeStart);

		// inserting data and sending host map change event
		InsertHostMapRecord(time, true, packet->ep());

		// checking for outgoing session with this host, if not exists - signal host status event
		m_IncomingLastUpdateTime = boost::posix_time::microsec_clock::local_time();
		m_IncomingStatus = status;

		SignalStatusEvent(status);
	}

private:

	//! Kernel reference
	IKernel&						m_Kernel;

	//! Logger reference
	ILog&							m_Log;

	//! Local host guid
	const std::string				m_LocalHostGuid;

	//! Remote host guid
	const std::string				m_RemoteHostGuid;

	//! Local ip
	const std::string				m_LocalIp;

	//! Local port
	const std::string				m_LocalPort;

	//! Ping interval
	std::size_t						m_PingInterval;

	//! Direct endpoint
	std::string						m_DirectEndpoint;

	//! NAT endpoint
	std::string						m_NATEndpoint;

	//! Host incoming status
	Status::Enum_t					m_IncomingStatus;

	//! Host outgoing status
	Status::Enum_t					m_OutgoingStatus;

	//! Incoming status last update time
	boost::posix_time::ptime		m_IncomingLastUpdateTime;

	//! Outgoing status last update time
	boost::posix_time::ptime		m_OutgoingLastUpdateTime;

	//! Host mutex
	boost::mutex					m_DataMutex;
};


CPingHost::CPingHost(IKernel& kernel, 
			 ILog& log, 
			 const std::string& localGuid,
			 const std::string& remoteGuid,
			 const std::string& localIp,
			 const std::string& localPort,
			 const std::size_t pingInterval)
	: m_pImpl(new Impl(kernel, log, localGuid, remoteGuid, localIp, localPort, pingInterval))
{
}

CPingHost::~CPingHost(void)
{
}

void CPingHost::DirectEndpoint(const std::string& ip, const std::string& port)
{
	m_pImpl->DirectEndpoint(ip, port);
}

void CPingHost::NATEndpoint(const std::string& ip, const std::string& port)
{
	m_pImpl->NATEndpoint(ip, port);
}

void CPingHost::Ping()
{
	m_pImpl->Ping();
}

void CPingHost::HandleRequest(const ProtoPacketPtr packet)
{
	m_pImpl->HandleRequest(packet);
}

void CPingHost::PingInterval(const std::size_t interval)
{
	m_pImpl->PingInterval(interval);
}

void CPingHost::IncomingStatus(const Status::Enum_t status)
{
	m_pImpl->IncomingStatus(status);
}

void CPingHost::OutgoingStatus(const Status::Enum_t status)
{
	m_pImpl->OutgoingStatus(status);
}
