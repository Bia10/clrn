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
		m_Log.Open("1");

		m_DBpath = conv::cast<std::string>(fs::FullPath(szDBpath ? szDBpath : KERNEL_DATABASE_FILE_NAME));
		CHECK(fs::Exists(m_DBpath), m_DBpath);
		DataBase::Create(m_Log, m_DBpath.c_str());

		m_pSettingsData.reset(new data::Table());
		m_pSettings.reset(new CSettings(m_Log, DataBase::Instance(), *m_pSettingsData));
		m_pSettings->Load();

		std::wstring logSource;
		m_pSettings->Get(KERNEL_MODULE_ID, logSource, "log_source");
		
		std::vector<ILog::Level::Enum_t> levels;
		for (std::size_t i = 0; i <= LAST_MODULE_ID; ++i)
		{
			int logLevel = 0;
			m_pSettings->Get(i, logLevel, "log_level");
			levels.push_back(static_cast<ILog::Level::Enum_t>(logLevel));
		}

		m_Log.Close();

		m_Log.SetLogLevels(levels);
		m_Log.Open(logSource.c_str());

		int port = 0;
		int threads = 0;
		int bufferSize = 0;
		m_pSettings->Get(KERNEL_MODULE_ID, port,		"udp_port");
		m_pSettings->Get(KERNEL_MODULE_ID, threads,		"udp_threads");
		m_pSettings->Get(KERNEL_MODULE_ID, bufferSize,	"udp_buffer_size");

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
	}
	CATCH_PASS_EXCEPTIONS("Init failed.")
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

void CKernel::Send(const std::string& ip, const std::string& port, const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	CHECK(packet);	

	TRY 
	{
		LOG_DEBUG("%s") % packet->DebugString();
		m_pServer->Send(ip, port, packet);
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
			boost::this_thread::sleep(boost::posix_time::milliseconds(500));

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
				LOG_WARNING("Job id: [%s], guid: [%s], timeout: [%s] ended.") % job->GetId() % job->GetGUID() % job->GetGUID();
				TRY 
				{
					job->HandleReply(ProtoPacketPtr());
				}
				CATCH_IGNORE_EXCEPTIONS(m_Log, job->GetId(), job->GetGUID(), job->GetGUID())
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
		ProtoPacketPtr packet;
		TRY 
		{
			packet = m_WorkQueue.PopFront();

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
						SendErrorReply(packet->from(), packet->guid(), e.what());
					}
					catch (...)
					{
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
		catch (const boost::thread_interrupted&)
		{
			LOG_WARNING("Work thread interrupted.");
			break;
		}
		CATCH_IGNORE_EXCEPTIONS(m_Log, "WorkThread failed.", packet ? *packet : packets::Packet())
	}
}

void CKernel::HandleNewPacket(const ProtoPacketPtr packet)
{
	m_WorkQueue.PushBack(packet);
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