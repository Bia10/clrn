#ifndef ExecuteScript_h__
#define ExecuteScript_h__

#include "BaseJob.h"

//! Execute script job
//!
//! \class CExecuteScript
//!
class CExecuteScript :
	public CBaseJob
{
public:
	CExecuteScript(IKernel& kernel, ILog& logger);

	//! Execute job
	void						Execute(const ProtoPacketPtr packet);
};

#endif // ExecuteScript_h__
