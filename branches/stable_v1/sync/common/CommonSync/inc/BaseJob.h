#ifndef BaseJob_h__
#define BaseJob_h__

#include "IJob.h"
#include "ILog.h"
#include "IKernel.h"

#include <boost/function.hpp>

class CBaseJob
	: public IJob
{
public:
	CBaseJob(IKernel& kernel, ILog& logger);
	~CBaseJob(void);

	//! Invoke job
	virtual void						Invoke(const TableList& params, const std::string& host);
	
	//! Execute job
	virtual void						Execute(const ProtoPacketPtr packet);

	//! Set callback function 
	virtual void						SetCallBack(const CallBackFn& callBack);

	//! Handle job reply
	virtual void						HandleReply(const ProtoPacketPtr packet);

	//! Job id
	virtual jobs::Job_JobId				GetId() const;

	//! Get job timeout
	virtual std::size_t					GetTimeout() const;

	//! Job packet guid
	virtual const std::string&			GetGUID() const;

protected:

	//! Kernel
	IKernel&			m_Kernel;

	//! Logger
	ILog&				m_Log;

	//! Callback function
	IJob::CallBackFn	m_CallBackFn;

	//! Job timeout
	std::size_t			m_TimeOut;

	//! Job id
	jobs::Job_JobId		m_Id;

	//! Packet guid
	std::string			m_GUID;
};
#endif // BaseJob_h__
