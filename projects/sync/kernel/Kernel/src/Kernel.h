#ifndef Kernel_h__
#define Kernel_h__

#include "IKernel.h"
#include "ILog.h"
#include "IJob.h"
#include "BlockingQueue.h"
#include "JobFactory.h"
#include "Log.h"
#include "PluginLoader.h"
#include "ProtoTablePtr.h"

#include <map>

#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>

//! Forward declarations
class CSettings;
namespace net
{
	class CUDPServer;
}

//! Kernel implementation
class CKernel
	: public IKernel
{
	//! Work queue type
	typedef CBlockingQueue<ProtoPacketPtr>		WorkQueue;

	//! Waiting executor descriptor
	struct WaitingJob
	{
		boost::posix_time::ptime	timeAdded;
		IJob::Ptr					job;
		std::size_t					timeout;
		std::string					host;
	};

	//! Type of the waiting jobs container
	typedef std::map<std::string, WaitingJob>		WaitingJobs;

	//! UDP server pointer type
	typedef boost::scoped_ptr<net::CUDPServer>		ServerPtr;

	//! Plugin loader ptr
	typedef boost::scoped_ptr<CPluginLoader>		PluginLoaderPtr;

	//! Mutex shared ptr type
	typedef boost::shared_ptr<boost::mutex>			MutexPtr;

	//! Settings pointer
	typedef boost::scoped_ptr<CSettings>			SettingsPtr;

public:
	CKernel(void);
	~CKernel(void);

	//! Init kernel
	void					Init(const char* szDBpath = 0);

	//! Run kernel
	void					Run();

	//! Handle new proto packet
	void					HandleNewPacket(const ProtoPacketPtr packet);

	//! Create job
	IJob::Ptr				CreateJob(const jobs::Job_JobId id);

	//! Add job to the list of waiting for reply executors
	void					AddToWaiting(const IJob::Ptr job, const std::string& host);

	//! Send packet
	void					Send(const std::string& destination, const ProtoPacketPtr packet);

	//! Send packet
	void					Send(const std::string& ip, const std::string& port, const ProtoPacketPtr packet);
	
	//! Execute job 
	void					ExecuteJob(const jobs::Job_JobId id, 
										const IJob::TableList& params, 
										const std::string& host = "",
										const IJob::CallBackFn& callBack = IJob::CallBackFn());
	
	//! Execute job 
	void					ExecuteJob(const jobs::Job_JobId id, 
										IKernel::TablesList& results,
										const IJob::TableList& params,
										const std::string& host = "");

	//! Database path
	const std::string&		DbPath() const;

private:

	//! Work loop
	void					WorkThread();

	//! Get waiting executor
	IJob::Ptr				GetWaitingJob(const std::string& guid);

	//! Load plugins
	void					LoadPlugins();

	//! Jobs callback
	void					JobCallBack(ProtoPacketPtr packet, 
										const MutexPtr mutex, 
										TablesList& results);

	//! Timeout controller thread
	void					TimeoutControllerThread();

	//! Send error packet
	void					SendErrorReply(const std::string& destination, const std::string& guid, const std::string& text);

	//! Hosts map callback
	void					HostMapCallBack(const ProtoPacketPtr packet);

	//! Hosts list callback
	void					HostListCallBack(const ProtoPacketPtr packet);

	//! Logger
	CLog					m_Log;

	//! Server guid
	std::string				m_LocalHostGuid;

	//! Active queue
	WorkQueue				m_WorkQueue;

	//! Jobs factory
	CJobFactory				m_Factory;

	//! Waiting for reply executors
	WaitingJobs				m_WaitingJobs;

	//! Waiting executors mutex
	boost::mutex			m_WaitingJobsMutex;

	//! Work thread group
	boost::thread_group		m_WorkPool;

	//! UDP server
	ServerPtr				m_pServer;

	//! Settings
	SettingsPtr				m_pSettings;

	//! Settings data
	ProtoTablePtr			m_pSettingsData;

	//! Plugin loader ptr
	PluginLoaderPtr			m_PluginLoader;

	//! Database path
	std::string				m_DBpath;

};
#endif // Kernel_h__
