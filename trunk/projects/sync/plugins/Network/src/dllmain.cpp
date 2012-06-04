#include "stdafx.h"
#include "NetworkPlugin.h"
#include "JobFactory.h"
#include "ILog.h"
#include "IKernel.h"
#include "FileSystem.h"
#include "job.pb.h"

#include "OutgoingPing.h"
#include "HostPinger.h"

#ifdef __cplusplus
extern "C" 
{
#endif

	NETWORK_API void Init(CJobFactory&, ILog&, IKernel&);
	NETWORK_API void Shutdown(CJobFactory&);

#ifdef __cplusplus
} 
#endif

CJobFactory* g_Factory;
void Init(CJobFactory& factory, ILog& logger, IKernel& kernel)
{
	factory.Register<COutgoingPing>(jobs::Job_JobId_PING_HOST);
	
	CHostPinger::Create(logger, kernel);
}

void Shutdown(CJobFactory& factory)
{
	CHostPinger::Shutdown();

	factory.Unregister(jobs::Job_JobId_PING_HOST);
}


