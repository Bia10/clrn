#ifndef Kernel_h__
#define Kernel_h__

#include "IKernel.h"
#include "ILog.h"
#include "IJob.h"
#include "BlockingQueue.h"
#include "JobFactory.h"
#include "Log.h"
#include "ProtoTablePtr.h"

#include <map>

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/scoped_ptr.hpp>

#include "PluginLoader.h"

namespace ba = boost::asio;

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
	//! Waiting executor descriptor
	struct WaitingJob
	{
		IJob::Ptr					job;
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

	//! Type of the boost::asio signal set pointer
	typedef boost::scoped_ptr<ba::signal_set> 		SignalSetPtr;

	//! Timer pointer
	typedef boost::shared_ptr<ba::deadline_timer>	TimerPtr;

public:
	CKernel(void);
	~CKernel(void);

	//! Init kernel
	void					Init(const char* szDBpath = 0);

	//! Run kernel
	void					Run();

	//! Stop kernel
	void					Stop();

private:

	//! Init log
	void					InitLog();

	//! Init UDP server
	void					InitUdpServer();

	//! Handle new proto packet
	void					HandleNewPacket(const ProtoPacketPtr packet);

	//! Add job to the list of waiting for reply executors
	void					AddToWaiting(const IJob::Ptr job, const std::string& host);

	//! Send packet
	void					Send(const std::string& destination, const ProtoPacketPtr packet);

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

	//! Work loop
	void					WorkThread();

	//! Get waiting executor
	IJob::Ptr				GetWaitingJob(const std::string& guid);

	//! Load plugins
	void					LoadPlugins();

	//! Load static plugins
	void					LoadStaticPlugins();

	//! Unload static plugins
	void					UnLoadStaticPlugins();

	//! Jobs callback
	void					JobCallBack(ProtoPacketPtr packet, 
										const MutexPtr mutex, 
										TablesList& results);

	//! Send error packet
	void					SendErrorReply(const std::string& destination, const std::string& guid, const std::string& text);

	//! Hosts map callback
	void					HostMapCallBack(const ProtoPacketPtr packet);

	//! Hosts list callback
	void					HostListCallBack(const ProtoPacketPtr packet);

	//! Host status callback
	void					HostStatusCallBack(const ProtoPacketPtr packet);

	//! Process packet
	void 					ProcessProtoPacket(const ProtoPacketPtr packet);

	//! Time event
	void					Timer(const boost::posix_time::time_duration interval, const TimeEventCallback callBack);

	//! Timer callback
	void					TimerCallBack(const boost::system::error_code& e, 
										const TimeEventCallback callback, 
										const TimerPtr timer);

	//! Waiting job timeout callback
	void					WaitingJobTimeoutCallback(const std::string& jobGuid);

	//! Settings interface
	ISettings&				Settings();

	//! Logger
	CLog					m_Log;

	//! Server guid
	std::string				m_LocalHostGuid;

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

	//! Service
	ba::io_service			m_Service;

	//! Service work
	ba::io_service::work	m_ServiceWork;

	//! Signals
	SignalSetPtr			m_pSignals;

};
#endif // Kernel_h__
