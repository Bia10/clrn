#ifndef GetSettings_h__
#define GetSettings_h__

#include "BaseJob.h"

//! Setting get job
//!
//! \class CGetSettings
//!
class CGetSettings :
	public CBaseJob
{
public:
	CGetSettings(IKernel& kernel, ILog& logger);

	//! Execute job
	void						Execute(const ProtoPacketPtr packet);

};
#endif // GetSettings_h__
