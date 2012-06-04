#include "stdafx.h"
#include "EventsPlugin.h"
#include "JobFactory.h"
#include "ILog.h"
#include "IKernel.h"
#include "FileSystem.h"
#include "job.pb.h"

#include "EventDispatcher.h"
#include "GetEvent.h"
#include "SetEvent.h"


#ifdef __cplusplus
extern "C" 
{
#endif

	EVENTS_API void Init(CJobFactory&, ILog&, IKernel&);
	EVENTS_API void Shutdown(CJobFactory&);

#ifdef __cplusplus
} 
#endif

void Init(CJobFactory& factory, ILog& logger, IKernel& kernel)
{	
	factory.Register<CGetEvent>(jobs::Job_JobId_GET_EVENT);
	factory.Register<CSetEvent>(jobs::Job_JobId_SET_EVENT);

	CEventDispatcher::Create(logger, kernel);
}

void Shutdown(CJobFactory& factory)
{
	CEventDispatcher::Shutdown();

	factory.Unregister(jobs::Job_JobId_GET_EVENT);
	factory.Unregister(jobs::Job_JobId_SET_EVENT);
}



