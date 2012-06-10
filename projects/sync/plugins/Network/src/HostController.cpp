#include "StdAfx.h"
#include "HostController.h"
#include "PingHost.h"


//! Host pinger implementation
//!
//! \class CHostController::Impl
//!
class CHostController::Impl
{

	//! Hosts map
	typedef std::map<std::string, CPingHost::Ptr>		HostMap;

public:

	Impl(ILog& logger, IKernel& kernel)
		: m_Log(logger)
		, m_Kernel(kernel)
		, m_PingInterval(0)
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

		// starting work thread
		m_WorkThread = boost::thread(boost::bind(&Impl::WorkLoop, this));
	}

	~Impl()
	{
		m_WorkThread.interrupt();
		m_WorkThread.join();
	}
	 
	//! Settings callback
	void SettingsCallback(const ProtoPacketPtr packet)
	{
		CHECK(packet);

		TRY 
		{
			boost::mutex::scoped_lock lock(m_HostMapMutex);
	
			CTable settings(*packet->mutable_job()->mutable_results(0));

			m_PingInterval = conv::cast<std::size_t>(settings["name=ping_interval"]["value"]);
		}
		CATCH_PASS_EXCEPTIONS(*packet)
	}

	//! Fill hosts
	void FillHosts(const CTable& hosts)
	{
		BOOST_FOREACH(const CTable::Row& row, hosts)
		{
			const std::string& guid = row["guid"];

			if (m_LocalHostGuid == guid)
				continue;

			HostMap::iterator it = m_HostMap.find(guid);

			if (m_HostMap.end() == it)
			{
				it = m_HostMap.insert(std::make_pair(guid, 
						CPingHost::Ptr(new CPingHost(
							m_Kernel,
							m_Log,
							m_LocalHostGuid,
							guid,
							m_LocalIp,
							m_LocalPort,
							m_PingInterval
						)))).first;

				LOG_TRACE("Adding host: [%s].") % guid;
			}
			else
			{
				LOG_TRACE("Updating host: [%s].") % guid;			
			}

			it->second->PingInterval(m_PingInterval);
			it->second->DirectEndpoint(row["ip"], row["port"]);
		}
	}

	//! Local hosts callback
	void LocalHostsCallback(const ProtoPacketPtr packet)
	{
		CHECK(packet);

		TRY 
		{	
			boost::mutex::scoped_lock lock(m_HostMapMutex);

			data::Table& data = *packet->mutable_job()->mutable_results(0);
			CTable hosts(data);

			// received full data first time
			if (m_LocalHostGuid.empty() && m_LocalIp.empty() && m_LocalPort.empty())
			{
				// local host guid
				m_LocalHostGuid = hosts["id=1"]["guid"];
				m_LocalIp		= hosts["id=1"]["ip"];
				m_LocalPort		= hosts["id=1"]["port"];
			}

			if (data::Table_Action_Insert == data.action())
			{
				FillHosts(hosts);
			}
			else
			if (data::Table_Action_Delete == data.action())
			{
				BOOST_FOREACH(const CTable::Row& row, hosts)
				{
					const std::string& guid = row["guid"];

					m_HostMap.erase(guid);

					LOG_TRACE("Deleting host: [%s].") % guid;
				}
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
			boost::mutex::scoped_lock lock(m_HostMapMutex);

			// initing statuses from received packet
			CTable table(*packet->mutable_job()->mutable_results(0));

			BOOST_FOREACH(const CTable::Row& row, table)
			{
				const std::string& from		= row["from"];
				const std::string& to		= row["to"];
				const std::size_t status = conv::cast<std::size_t>(row["status"]);

				if (from == m_LocalHostGuid)
				{
					m_HostMap[to]->OutgoingStatus(static_cast<CPingHost::Status::Enum_t>(status));
					m_HostMap[to]->NATEndpoint(row["ip"], row["port"]);
				}
				else
				if (to == m_LocalHostGuid)
					m_HostMap[from]->IncomingStatus(static_cast<CPingHost::Status::Enum_t>(status));
			}		
		}
		CATCH_PASS_EXCEPTIONS(*packet)
	}

	//! Work thread
	void WorkLoop()
	{	
		SCOPED_LOG(m_Log);

		LOG_TRACE("CHostspinger work thread started, hosts to ping: [%s]") % m_HostMap.size();

		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

		// get hosts
		CProcedure script(m_Kernel);
		CProcedure::ParamsMap params;
		script.Execute(CProcedure::Id::HostsLoad, params, boost::bind(&Impl::LocalHostsCallback, this, _1));

		for (;;)
		{
			TRY 
			{
				// sleep for ping interval
				boost::this_thread::sleep(boost::posix_time::milliseconds(m_PingInterval));

				boost::mutex::scoped_lock lock(m_HostMapMutex);

				// pinging each host
				BOOST_FOREACH(const HostMap::value_type& host, m_HostMap)
				{
					host.second->Ping();
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

		boost::mutex::scoped_lock lock(m_HostMapMutex);

		const HostMap::iterator itHost = m_HostMap.find(packetGuid);

		if (m_HostMap.end() != itHost)
		{
			itHost->second->HandleRequest(packet);
			return;
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

	//! Host map mutex
	boost::mutex				m_HostMapMutex;

	//! Host map
	HostMap						m_HostMap;

};


boost::scoped_ptr<CHostController> CHostController::s_pInstance;

CHostController::CHostController(ILog& logger, IKernel& kernel)
	: m_pImpl(new Impl(logger, kernel))
{

}

CHostController::~CHostController(void)
{
}

CHostController& CHostController::Instance()
{
	return *s_pInstance;
}

void CHostController::Create(ILog& logger, IKernel& kernel)
{
	s_pInstance.reset(new CHostController(logger, kernel));
}

void CHostController::Shutdown()
{
	s_pInstance.reset();
}

void CHostController::OnRemotePingReceived(const ProtoPacketPtr packet)
{
	m_pImpl->OnRemotePingReceived(packet);
}
