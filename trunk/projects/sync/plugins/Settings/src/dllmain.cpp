#include "stdafx.h"
#include "SettingsPlugin.h"
#include "JobFactory.h"
#include "ILog.h"
#include "IKernel.h"
#include "GetSettings.h"
#include "SetSettings.h"
#include "job.pb.h"


#ifdef __cplusplus
extern "C"  
{
#endif

	SETTINGS_API void Init(CJobFactory&, ILog&, IKernel&);
	SETTINGS_API void Shutdown(CJobFactory& factory); 

#ifdef __cplusplus
}
#endif

void Init(CJobFactory& factory, ILog& /*logger*/, IKernel& /*kernel*/)
{
	factory.Register<CSetSettings>(jobs::Job_JobId_SET_SETTINGS); 
	factory.Register<CGetSettings>(jobs::Job_JobId_GET_SETTINGS);
}

void Shutdown(CJobFactory& factory)
{
	factory.Unregister(jobs::Job_JobId_SET_SETTINGS);
	factory.Unregister(jobs::Job_JobId_GET_SETTINGS);
}


