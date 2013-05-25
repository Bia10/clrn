#ifndef ExecuteProcedure_h__
#define ExecuteProcedure_h__

#include "BaseJob.h"

//! Execute procedure job
//!
//! \class CExecuteScript
//!
class CExecuteProcedure :
	public CBaseJob
{
public:
	CExecuteProcedure(IKernel& kernel, ILog& logger);

	//! Execute job
	void						Execute(const ProtoPacketPtr packet);
};

#endif // ExecuteProcedure_h__
