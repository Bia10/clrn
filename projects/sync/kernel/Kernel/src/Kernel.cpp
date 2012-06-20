#include "stdafx.h"
#include "Kernel.h"


CKernel::CKernel()
{ 

}

CKernel::~CKernel(void)
{
	SCOPED_LOG(m_Log);

	DataBase::Shutdown();

	if (m_pServer)
		m_pServer->Stop();
	m_Log.Close();
}

void CKernel::Run()
{
	SCOPED_LOG(m_Log);
	CHECK(m_pServer);

	m_pServer->Run();

	// shutdown initiated
	m_PluginLoader->Shutdown();
	m_WorkPool.interrupt_all();
	m_WorkPool.join_all();

	{
		boost::mutex::scoped_lock lock(m_WaitingJobsMutex);
		m_WaitingJobs.clear();
	}

	m_PluginLoader->Unload();
	m_pSettings.reset();
	m_WorkQueue.Clear();
}

const std::string& CKernel::DbPath() const
{
	return m_DBpath;
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

		m_DBpath = conv::cast<std::string>(fs::FullPath(szDBpath ? szDBpath : KERNEL_DATABASE_FILE_NAME));
		CHECK(fs::Exists(m_DBpath), m_DBpath);
		DataBase::Create(m_Log, m_DBpath.c_str());

		m_pSettingsData.reset(new data::Table());
		m_pSettings.reset(new CSettings(m_Log, DataBase::Instance(), *m_pSettingsData));
		m_pSettings->Load();
		
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

		int port = 0;
		int threads = 0;
		int bufferSize = 0;
		m_pSettings->Get(KERNEL_MODULE_ID, port,		"udp_port");
		m_pSettings->Get(KERNEL_MODULE_ID, threads,		"udp_threads");
		m_pSettings->Get(KERNEL_MODULE_ID, bufferSize,	"udp_buffer_size");

		std::size_t pingInterval = 0;
		m_pSettings->Get(KERNEL_MODULE_ID, pingInterval, "ping_interval");

		// getting hosts
		data::Table hostsTableData;
		DataBase::Instance().Execute((boost::format("select * from %s") % HOSTS_TABLE_NAME).str().c_str(), hostsTableData, data::Table_Id_Hosts);
		CTable hostsTable(hostsTableData);

		// getting local host guid
		m_LocalHostGuid = hostsTable["id=1"]["guid"];

		m_pServer.reset
		(
			new net::CUDPServer
			(
				m_Log, 
				*this, 
				port, 
				threads, 
				bufferSize,
				m_LocalHostGuid
			)
		);

		// setting up ping interval
		m_pServer->SetPingInterval(pingInterval);

		// setting up server endpoints
		const ProtoPacketPtr hostList(new packets::Packet());
		*hostList->mutable_job()->add_results() = hostsTableData;
		HostListCallBack(hostList);

		int workPoolSize = 0;
		m_pSettings->Get(KERNEL_MODULE_ID, workPoolSize, "kernel_threads");
		for (int i = 0; i < workPoolSize; ++i)
			m_WorkPool.create_thread(boost::bind(&CKernel::WorkThread, this));

		m_WorkPool.create_thread(boost::bind(&CKernel::TimeoutControllerThread, this));

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
	}
	CATCH_PASS_EXCEPTIONS("Init failed.")
}

void CKernel::HostStatusCallBack(const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	CHECK(packet);

	TRY 
	{
		data::Table* protoTable = packet->mutable_job()->mutable_results(0);
		CTable table(*protoTable);
		const std::string& guid = table[0]["guid"];

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
			LOG_WARNING("Job id: [%s], guid: [%s], erased due host went offline.") % job->GetId() % job->GetGUID() % job->GetGUID();
			TRY 
			{
				job->HandleReply(ProtoPacketPtr());
			}
			CATCH_IGNORE_EXCEPTIONS(m_Log, job->GetId(), job->GetGUID(), job->GetGUID())
		}
	}
	CATCH_PASS_EXCEPTIONS(*packet)
}

void CKernel::HostListCallBack(const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	CHECK(packet);

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
		LOG_DEBUG("%s") % packet->DebugString();
		
		if (destination.empty() || destination == m_LocalHostGuid)
 		{
 			HandleNewPacket(packet);
 			return;
 		}

		m_pServer->Send(destination, packet);
	}
	CATCH_PASS_EXCEPTIONS(destination, *packet)
}

void CKernel::Send(const std::string& destination, const std::string& ip, const std::string& port, const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	CHECK(packet);	

	TRY 
	{
		LOG_DEBUG("%s") % packet->DebugString();
		m_pServer->Send(destination, ip, port, packet);
	}
	CATCH_PASS_EXCEPTIONS(ip, port, *packet)
}

IJob::Ptr CKernel::CreateJob(const jobs::Job_JobId id)
{
	return m_Factory.Create(id, *this, m_Log);
}

void CKernel::AddToWaiting(const IJob::Ptr job, const std::string& host)
{
	WaitingJob descriptor;
	descriptor.job			= job;
	descriptor.timeAdded	= boost::posix_time::microsec_clock::local_time();
	descriptor.timeout		= job->GetTimeout();
	descriptor.host			= host;

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

void CKernel::TimeoutControllerThread()
{
	SCOPED_LOG(m_Log);

	for (;;)
	{
		TRY 
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(100));

			CheckTimeEvents();

			const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		
			std::vector<IJob::Ptr> jobsToErase;
			{
				boost::mutex::scoped_lock lock(m_WaitingJobsMutex);

				WaitingJobs::const_iterator it = m_WaitingJobs.begin();
				const WaitingJobs::const_iterator itEnd = m_WaitingJobs.end();
				for (; it != itEnd; ++it)
				{
					const std::size_t timeout = it->second.timeout;
					if (std::size_t(-1) == timeout)
						continue; // infinite timeout

					boost::posix_time::time_duration td = now - it->second.timeAdded;

					if (td.total_milliseconds() < timeout)
						continue;

					jobsToErase.push_back(it->second.job);

					m_WaitingJobs.erase(it);			
					it = m_WaitingJobs.begin();
				}
			}

			BOOST_FOREACH(const IJob::Ptr& job, jobsToErase)
			{
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
		}
		catch (const boost::thread_interrupted&)
		{
			LOG_WARNING("TimeoutControllerThread interrupted.");
			break;
		}
		CATCH_IGNORE_EXCEPTIONS(m_Log, "TimeoutControllerThread failed.")
	}
}

void CKernel::WorkThread()
{
	SCOPED_LOG(m_Log);

	for (;;)
	{
		TRY 
		{
			const KernelJob job = m_WorkQueue.PopFront();
			job();
		}
		catch (const boost::thread_interrupted&)
		{
			LOG_WARNING("Work thread interrupted.");
			break;
		}
		CATCH_IGNORE_EXCEPTIONS(m_Log, "WorkThread failed.")
	}
}

void CKernel::ProcessProtoPacket(const ProtoPacketPtr packet)
{
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
			job->HandleReply(packet);
		}
		break;
	default:
		assert(false);
	}
}

void CKernel::HandleNewPacket(const ProtoPacketPtr packet)
{
	m_WorkQueue.PushBack(boost::bind(&CKernel::ProcessProtoPacket, this, packet));
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
		packet->mutable_job()->add_results()->add_rows()->add_data(text);
		packet->set_type(packets::Packet_PacketType_ERR);
		packet->set_guid(guid);

		Send(destination, packet);		
	}
	CATCH_IGNORE_EXCEPTIONS(m_Log, "CKernel::SendErrorReply failed.", destination, guid, text)
}

void CKernel::TimeEvent(const boost::posix_time::time_duration interval, const TimeEventCallback callBack, const bool periodic)
{
	boost::mutex::scoped_lock lock(m_TimeEventsMutex);

	TimeEventDsc e;
	e.timeAdded = boost::posix_time::microsec_clock::local_time();
	e.interval = interval;
	e.periodic = periodic;
	e.callback = callBack;

	const TimeEvents::iterator it = std::find(m_TimeEvents.begin(), m_TimeEvents.end(), callBack);
	if (m_TimeEvents.end() != it)
	{
		if (interval.total_milliseconds())
			*it = e; // change callback
		else
			m_TimeEvents.erase(it); // remove event
	}
	else 
	if (interval.total_milliseconds())
		m_TimeEvents.push_back(e); // add new event
}

void CKernel::CheckTimeEvents()
{
	TRY 
	{
		boost::mutex::scoped_lock lock(m_TimeEventsMutex);

		const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

		TimeEvents::iterator it = m_TimeEvents.begin();
		const TimeEvents::const_iterator itEnd = m_TimeEvents.end();
		for (; it != itEnd;)
		{
			TimeEventDsc& e = *it;
			if (e.timeAdded + e.interval < now)
			{
				// timeout expired, add this item to work queue
				m_WorkQueue.PushBack(e.callback);

				if (e.periodic)
				{
					e.timeAdded = now;
				}
				else
				{
					it = m_TimeEvents.erase(it);
					continue;
				}
			}

			++it;
		}
	}
	CATCH_IGNORE_EXCEPTIONS(m_Log)
}

bool CKernel::TimeEventDsc::operator == (const TimeEventCallback& that) const
{
	return callback.target_type() == that.target_type();
}
