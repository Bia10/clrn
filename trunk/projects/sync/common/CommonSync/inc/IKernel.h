#ifndef IKernel_h__
#define IKernel_h__

#include "IJob.h"
#include "ProtoTablePtr.h"

#include <string>
#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

//! Kernel interface
//!
//! \class IKernel
//!
class IKernel
{
public:

	//! List of proto table ptrs
	typedef std::list<ProtoTablePtr>				TablesList;

	//! Handle new proto packet
	virtual void				HandleNewPacket(const ProtoPacketPtr packet) = 0;

	//! Create job
	virtual IJob::Ptr			CreateJob(const jobs::Job_JobId id) = 0;

	//! Add executor to the list of waiting for reply executors
	virtual void				AddToWaiting(const IJob::Ptr job, const std::string& host) = 0;

	//! Send packet
	virtual void				Send(const std::string& destination, const ProtoPacketPtr packet) = 0;

	//! Send packet
	virtual void				Send(const std::string& destination, const std::string& ip, const std::string& port, const ProtoPacketPtr packet) = 0;

	//! Execute job 
	virtual void				ExecuteJob(const jobs::Job_JobId id, 
											const IJob::TableList& params, 
											const std::string& host = "",
											const IJob::CallBackFn& callBack = IJob::CallBackFn()) = 0;
	
	//! Execute job 
	virtual void				ExecuteJob(const jobs::Job_JobId id, 
											IKernel::TablesList& results,
											const IJob::TableList& params,
											const std::string& host = "") = 0;

	//! Database path
	virtual const std::string&	DbPath() const = 0;

	//! Destructor
	virtual ~IKernel() {}
};

#endif // IKernel_h__