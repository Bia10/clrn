#ifndef GetEvent_h__
#define GetEvent_h__

#include "BaseJob.h"

//! Get event job
//!
//! \class CGetEvent
//!
class CGetEvent :
	public CBaseJob
{
public:
	CGetEvent(IKernel& kernel, ILog& logger);

	//! Execute job
	void			Execute(const ProtoPacketPtr packet);

	//! Event completion callback
	void			EventCallBack(const ProtoPacketPtr packet);			

	//! Handle job reply
	void			HandleReply(const ProtoPacketPtr packet);

private:

	//! Request packet guid
	std::string				m_RequestPacketGuid;

	//! Request packet host
	std::string				m_RequestPacketHost;
};
#endif // GetEvent_h__