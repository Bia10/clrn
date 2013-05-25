#ifndef IJob_h__
#define IJob_h__

#include "ILog.h"
#include "ProtoJobPtr.h"
#include "ProtoPacketPtr.h"
#include "ProtoTablePtr.h"

#include <list>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function/function_fwd.hpp>

//! Forward declarations
class IKernel;

//! Job interface
//!
//! \class IJob
//! 
class IJob 
	: private boost::noncopyable
	, public boost::enable_shared_from_this<IJob>
{
public:

	//! Job pointer type
	typedef boost::shared_ptr<IJob>							Ptr;

	//! Callback type
	typedef boost::function<void (const ProtoPacketPtr)>	CallBackFn;

	//! Auto ptr type
	typedef std::auto_ptr<data::Table>						AutoPtr;

	//! Smart table ptr type
	typedef boost::shared_ptr<AutoPtr>						TablePtr;

	//! Tables list
	typedef std::list<TablePtr>								TableList;

	//! Invoke job
	virtual void						Invoke(const TableList& params, const std::string& host) = 0;

	//! Execute job
	virtual void						Execute(const ProtoPacketPtr packet) = 0;

	//! Handle job reply
	virtual void						HandleReply(const ProtoPacketPtr packet) = 0;

	//! Set callback function 
	virtual void						SetCallBack(const CallBackFn& callBack) = 0;

	//! Job id
	virtual jobs::Job_JobId				GetId() const = 0;

	//! Job packet guid
	virtual const std::string&			GetGUID() const = 0;

	//! Get job timeout
	virtual std::size_t					GetTimeout() const = 0;

	//! Destructor
	virtual ~IJob() {}

};


#endif // IJob_h__