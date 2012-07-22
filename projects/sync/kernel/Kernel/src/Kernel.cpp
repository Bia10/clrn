#include "stdafx.h"
#include "Kernel.h"

#include <boost/asio/ip/host_name.hpp>

CKernel::CKernel()
	: m_ServiceWork(m_Service)
{ 

}

CKernel::~CKernel(void)
{
	SCOPED_LOG(m_Log);

	DataBase::Shutdown();

	m_Log.Close();
}

void CKernel::Run()
{
	SCOPED_LOG(m_Log);
	m_Service.run();
}

void CKernel::Stop()
{
	SCOPED_LOG(m_Log);

	LOG_WARNING("Stop signal catched. Stopping kernel...");

	m_PluginLoader->Shutdown();

	m_Service.stop();
	m_WorkPool.join_all();

	{
		boost::mutex::scoped_lock lock(m_WaitingJobsMutex);
		m_WaitingJobs.clear();
	}

	m_PluginLoader->Unload();
	m_pSettings.reset();
	m_pServer.reset();

	LOG_WARNING("Kernel stopped.");
}

ISettings& CKernel::Settings()
{
	return *m_pSettings;
}

void CKernel::InitLog()
{
	TRY 
	{
		std::vector<ILog::Level::Enum_t> levels;
		for (std::size_t i = 0; i <= LAST_MODULE_ID; ++i)
		{
			int logLevel = 0;
			m_pSettings->Get(i, logLevel, "log_level"); 
			levels.push_back(static_cast<ILog::Level::Enum_t>(logLevel));
		}
	
		m_Log.Close();
	
		for (std::size_t i = 0; i <= LAST_MODULE_ID; ++i)
		{
			std::string logSource;
			m_pSettings->Get(i, logSource, "log_source");
			m_Log.Open(logSource, i);
		}	
	
		m_Log.SetLogLevels(levels);
	}
	CATCH_PASS_EXCEPTIONS("Failed to init log")
}

void CKernel::InitUdpServer()
{
	TRY 
	{
		m_pServer.reset
		(
			new net::CUDPServer
			(
				m_Log, 
				*this, 
				m_Service
			)
		);
	
		// Creating signal set
		m_pSignals.reset(new boost::asio::signal_set(m_Service));
	
		m_pSignals->add(SIGINT);
		m_pSignals->add(SIGTERM);
	
	#if defined(SIGQUIT)
		m_pTerminationSignals->add(SIGQUIT);
	#endif // defined(SIGQUIT)
	
		m_pSignals->async_wait(boost::bind(&CKernel::Stop, this));
		}
	CATCH_PASS_EXCEPTIONS("Failed to init udp server")
}

void CKernel::LoadPlugins()
{
	SCOPED_LOG(m_Log);

	std::string pluginsPath;
	TRY 
	{
		m_pSettings->Get(KERNEL_MODULE_ID, pluginsPath, "plugins_path");
	
		CPluginLoader::TPlugins plugins;
	
		data::Table table;
		DataBase::Instance().Execute("select name from plugins", table);
		
		CTable pluginsTable(table);
		BOOST_FOREACH(const CTable::Row& row, pluginsTable)
		{
			plugins.push_back(row[0]);
		}
					
		m_PluginLoader.reset(new CPluginLoader(m_Log, *this, m_Factory, pluginsPath, plugins));
		m_PluginLoader->Load();
	}
	CATCH_PASS_EXCEPTIONS("LoadPlugins failed.", pluginsPath)
}

void CKernel::Init(const char* szDBpath /*= 0*/)
{	
	SCOPED_LOG(m_Log);

	TRY 
	{
		m_Log.Open("1", CURRENT_MODULE_ID);

		const std::string dbPath = conv::cast<std::string>(fs::FullPath(szDBpath ? szDBpath : KERNEL_DATABASE_FILE_NAME));
		CHECK(fs::Exists(dbPath), dbPath);
		DataBase::Create(m_Log, dbPath.c_str());

		m_pSettingsData.reset(new data::Table());
		m_pSettings.reset(new CSettings(m_Log, DataBase::Instance(), *m_pSettingsData));
		m_pSettings->Load();
		m_pSettings->SetDBpath(dbPath);

		// init log
		InitLog();

		// local host name
		const std::string localHostName = ba::ip::host_name();

		// update host map table
		DataBase::Instance().Execute((boost::format("update %s set ip = '%s' where id = 1") % HOSTS_TABLE_NAME % localHostName).str().c_str());

		// getting hosts
		data::Table hostsTableData;
		DataBase::Instance().Execute((boost::format("select * from %s") % HOSTS_TABLE_NAME).str().c_str(), hostsTableData, data::Table_Id_Hosts);
		CTable hostsTable(hostsTableData);

		// getting local host guid
		m_LocalHostGuid = hostsTable["id=1"]["guid"];
		m_pSettings->SetLocalGuid(m_LocalHostGuid);

		LOG_TRACE("Local host name: [%s], GUID: [%s]") % localHostName % m_LocalHostGuid;

		// init udp server
		InitUdpServer();

		// load plugins
		LoadPlugins();

		// getting host mapping
		CProcedure hostMapping(*this);
		hostMapping.Execute(CProcedure::Id::HostMapLoad, CProcedure::ParamsMap(), boost::bind(&CKernel::HostMapCallBack, this, _1));

		// subscribing to host map change event
		CEvent hostMapEvent(*this, HOSTMAP_EVENT_NAME);
		hostMapEvent.Subscribe(boost::bind(&CKernel::HostMapCallBack, this, _1));

		// subscribing to host list change event
		CEvent hostListEvent(*this, HOSTS_TABLE_NAME);
		hostListEvent.Subscribe(boost::bind(&CKernel::HostListCallBack, this, _1));

		// subscribing to host status event
		CEvent hostStatusEvent(*this, HOST_STATUS_EVENT_NAME);
		hostStatusEvent.Subscribe(boost::bind(&CKernel::HostStatusCallBack, this, _1));

		// setting up server endpoints
		CProcedure hostList(*this);
		hostList.Execute(CProcedure::Id::HostsLoad, CProcedure::ParamsMap(), boost::bind(&CKernel::HostListCallBack, this, _1));

		const std::size_t size = m_pSettings->ThreadsCount();
		for (std::size_t i = 0; i < size; ++i)
			m_WorkPool.create_thread(boost::bind(&boost::asio::io_service::run, &m_Service));
	}
	CATCH_PASS_EXCEPTIONS("Init failed.")
}

void CKernel::HostStatusCallBack(const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	CHECK(packet);

	TRACE_PACKET(packet);

	TRY 
	{
		data::Table* protoTable = packet->mutable_job()->mutable_results(0);
		CTable table(*protoTable);
		const std::string& guid = table[0]["guid"];

		LOG_TRACE("Catched host status event, action: [%s]. Guid: [%s]") % protoTable->action() % guid;

		if (data::Table_Action_Insert == protoTable->action())
		{
			LOG_WARNING("Host: [%s] has come online.") % guid;
			return;
		}

		if (data::Table_Action_Delete != protoTable->action())
			return;

		LOG_WARNING("Host: [%s] went offline, deleting all associated jobs.") % guid;

		// deleting all jobs associated with this host
		std::vector<IJob::Ptr> jobsToErase;
		{
			boost::mutex::scoped_lock lock(m_WaitingJobsMutex);

			WaitingJobs::const_iterator it = m_WaitingJobs.begin();
			const WaitingJobs::const_iterator itEnd = m_WaitingJobs.end();
			for (; it != itEnd; ++it)
			{
				if (it->second.host != guid)
					continue;

				jobsToErase.push_back(it->second.job);

				m_WaitingJobs.erase(it);			
				it = m_WaitingJobs.begin();
			}
		}

		BOOST_FOREACH(const IJob::Ptr& job, jobsToErase)
		{
			LOG_WARNING("Job id: [%s], guid: [%s], erased due host went offline.") % job->GetId() % job->GetGUID();
			TRY 
			{
				job->HandleReply(ProtoPacketPtr());
			}
			CATCH_IGNORE_EXCEPTIONS(m_Log, job->GetId(), job->GetGUID())
		}
	}
	CATCH_PASS_EXCEPTIONS(*packet)
}

void CKernel::HostListCallBack(const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	CHECK(packet);

	TRACE_PACKET(packet);

	TRY 
	{
		m_pServer->AddHosts(packet);
	}
	CATCH_PASS_EXCEPTIONS(*packet)
}

void CKernel::HostMapCallBack(const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	CHECK(packet);

	TRACE_PACKET(packet);
	
	TRY 
	{
		data::Table* data = packet->mutable_job()->mutable_results(0);

		if (data::Table_Action_Insert == data->action())
		{
			m_pServer->AddHostMapping(packet);
		}
		else
		if (data::Table_Action_Delete == data->action())
		{
			m_pServer->RemoveHostMapping(packet);
		}		
	}
	CATCH_PASS_EXCEPTIONS(*packet)
}

void CKernel::Send(const std::string& destination, const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	CHECK(packet);	

	TRY 
	{
		LOG_DEBUG("%s") % packet->ShortDebugString();
		
		if (destination.empty() || destination == m_LocalHostGuid)
 		{
 			HandleNewPacket(packet);
 			return;
 		}

		m_pServer->Send(destination, packet);
	}
	CATCH_PASS_EXCEPTIONS(destination, *packet)
}

void CKernel::AddToWaiting(const IJob::Ptr job, const std::string& host)
{
	WaitingJob descriptor;
	descriptor.job				= job;
	descriptor.host				= host;
	const std::size_t timeout	= job->GetTimeout();

	if (timeout && timeout != std::numeric_limits<std::size_t>::max())
		Timer(boost::posix_time::milliseconds(timeout), boost::bind(&CKernel::WaitingJobTimeoutCallback, this, job->GetGUID()));

	boost::mutex::scoped_lock lock(m_WaitingJobsMutex);
	m_WaitingJobs.insert(std::make_pair(job->GetGUID(), descriptor));
}

IJob::Ptr CKernel::GetWaitingJob(const std::string& guid)
{
	boost::mutex::scoped_lock lock(m_WaitingJobsMutex);

	const WaitingJobs::const_iterator it = m_WaitingJobs.find(guid);
	CHECK(m_WaitingJobs.end() != it, guid);

	const IJob::Ptr job = it->second.job;
	m_WaitingJobs.erase(it);
	return job;
}

void CKernel::ProcessProtoPacket(const ProtoPacketPtr packet)
{
	if (!packet)
		return;

	TRY 
	{
		TRACE_PACKET(packet);

		switch(packet->type())
		{
		case packets::Packet_PacketType_REQUEST:
			{
				try
				{
					const IJob::Ptr job = m_Factory.Create(packet->job().id(), *this, m_Log);
					job->Execute(packet);
				}
				catch (const std::exception& e)
				{
					if (packets::Packet_PacketType_REQUEST != packet->type())
						throw;
	
					SendErrorReply(packet->from(), packet->guid(), e.what());
				}
				catch (...)
				{
					if (packets::Packet_PacketType_REQUEST != packet->type())
						throw;
	
					SendErrorReply(packet->from(), packet->guid(), "Unhandled exception.");
				}
			}
			break;
		case packets::Packet_PacketType_REPLY:
		case packets::Packet_PacketType_ERR:	
			{
				const IJob::Ptr job = GetWaitingJob(packet->guid());
				if (job)
					job->HandleReply(packet);
			}
			break;
		default:
			assert(false);
		}
	}
	CATCH_IGNORE_EXCEPTIONS(m_Log, "ProcessProtoPacket failed.", *packet)
}

void CKernel::HandleNewPacket(const ProtoPacketPtr packet)
{
	CHECK(packet);
	LOG_DEBUG("Queuing new packet: [%s].") % packet->ShortDebugString();
	TRACE_PACKET(packet);
	m_Service.post(boost::bind(&CKernel::ProcessProtoPacket, this, packet));
}

void CKernel::ExecuteJob(const jobs::Job_JobId id, const IJob::TableList& params, const std::string& host /*= ""*/, const IJob::CallBackFn& callBack /*= IJob::CallBackFn()*/)
{
	SCOPED_LOG(m_Log);

	TRY 
	{
		const IJob::Ptr job(m_Factory.Create(id, *this, m_Log));
		job->SetCallBack(callBack);
		job->Invoke(params, host);
	}
	CATCH_PASS_EXCEPTIONS("ExecuteJob async failed.", id, host)
}

void CKernel::ExecuteJob(const jobs::Job_JobId id, IKernel::TablesList& results, const IJob::TableList& params, const std::string& host /*= ""*/)
{
	SCOPED_LOG(m_Log);

	TRY 
	{
		const MutexPtr mutex(new boost::mutex());
		mutex->lock();

		IJob::CallBackFn callBack(boost::bind(&CKernel::JobCallBack, this, _1, mutex, boost::ref(results)));

		const IJob::Ptr job(m_Factory.Create(id, *this, m_Log));
		job->SetCallBack(callBack);
		job->Invoke(params, host);

		mutex->timed_lock(boost::posix_time::milliseconds(job->GetTimeout()));
	}
	CATCH_PASS_EXCEPTIONS("ExecuteJob sync failed.", id, host)
}

void CKernel::JobCallBack(const ProtoPacketPtr packet, const MutexPtr mutex, TablesList& results)
{
	SCOPED_LOG(m_Log);

	if (!packet)
	{
		mutex->unlock();
		return;
	}

	jobs::Job& job = *packet->mutable_job();
	google::protobuf::RepeatedPtrField<data::Table>& tables = *job.mutable_results();
	while (tables.size())
	{
		const ProtoTablePtr table(tables.ReleaseLast());
		results.push_front(table);
	}

	mutex->unlock();
}

void CKernel::SendErrorReply(const std::string& destination, const std::string& guid, const std::string& text)
{
	SCOPED_LOG(m_Log);

	TRY 
	{
		const ProtoPacketPtr packet(new packets::Packet());
		TRACE_PACKET(packet);

		packet->mutable_job()->add_results()->add_rows()->add_data(text);
		packet->set_type(packets::Packet_PacketType_ERR);
		packet->set_guid(guid);

		Send(destination, packet);		
	}
	CATCH_IGNORE_EXCEPTIONS(m_Log, "CKernel::SendErrorReply failed.", destination, guid, text)
}

void CKernel::Timer(const boost::posix_time::time_duration interval, const TimeEventCallback callBack)
{
	const TimerPtr timer(new ba::deadline_timer(m_Service));
	timer->expires_from_now(interval);
	timer->async_wait(boost::bind(&CKernel::TimerCallBack, this, ba::placeholders::error, callBack, timer));
}

void CKernel::TimerCallBack(const boost::system::error_code& e, const TimeEventCallback callback, const TimerPtr timer)
{
	TRY 
	{
		CHECK(callback);
		CHECK(!e, e.message(), callback.target_type().name());
		m_Service.post(callback);
	}
	CATCH_IGNORE_EXCEPTIONS(m_Log, "TimerCallBack failed", boost::posix_time::to_iso_extended_string(timer->expires_at()))
}

void CKernel::WaitingJobTimeoutCallback(const std::string& jobGuid)
{
	IJob::Ptr job;

	{
		boost::mutex::scoped_lock lock(m_WaitingJobsMutex);
		const WaitingJobs::const_iterator it = m_WaitingJobs.find(jobGuid);
		if (m_WaitingJobs.end() == it)
			return;

		job = it->second.job;

		m_WaitingJobs.erase(jobGuid);
	}

	LOG_WARNING("Job id: [%s], guid: [%s], timeout: [%s] ended.") 
		% job->GetId() 
		% job->GetGUID()
		% job->GetTimeout();

	TRY 
	{
		job->HandleReply(ProtoPacketPtr());
	}
	CATCH_IGNORE_EXCEPTIONS(m_Log, job->GetId(), job->GetTimeout(), job->GetGUID())
}
