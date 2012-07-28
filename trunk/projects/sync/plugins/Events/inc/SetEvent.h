#ifndef SetEvent_h__
#define SetEvent_h__

#include "BaseJob.h"


//! Event invoke job
//!
//! \class CSetEvent
//!
class CSetEvent :
	public CBaseJob
{
public:
	CSetEvent(IKernel& kernel, ILog& logger);
	
	//! Invoke job
	void				Invoke(const TableList& params, const std::string& host);

	//! Execute job
	void				Execute(const ProtoPacketPtr packet);

};
#endif // SetEvent_h__
