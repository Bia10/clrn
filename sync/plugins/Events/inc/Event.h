#ifndef Event_h__
#define Event_h__

#include "IJob.h"
#include "IKernel.h"

#include <string>

#include <boost/noncopyable.hpp>

//! Event implementation
class CEvent : boost::noncopyable
{
public:

	CEvent(IKernel& kernel, const std::string& name);
	~CEvent(void);

	//! Subscribe to event
	void		Subscribe(const IJob::CallBackFn& callback, const std::string& hash, const std::string& host = "");

	//! Signal event
	void		Signal(const ProtoPacketPtr packet, const IJob::CallBackFn callback = IJob::CallBackFn());

private:

	//! Kernel reference
	IKernel&				m_Kernel;

	//! Name
	const std::string		m_Name;
};
#endif // Event_h__
