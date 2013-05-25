#ifndef SetSettings_h__
#define SetSettings_h__

#include "BaseJob.h"

//! Setting set job
//!
//! \class CSetSettings
//!
class CSetSettings :
	public CBaseJob
{
public:
	CSetSettings(IKernel& kernel, ILog& logger);

	//! Execute job
	void						Execute(const ProtoPacketPtr packet);

};
#endif // GetSettings_h__
