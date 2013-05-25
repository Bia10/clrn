#include "stdafx.h"
#include "NetworkPlugin.h"
#include "JobFactory.h"
#include "ILog.h"
#include "IKernel.h"
#include "FileSystem.h"
#include "job.pb.h"

#include "Connect.h"
#include "ConnectionEstablisher.h"

#ifdef __cplusplus
extern "C" 
{
#endif

	NETWORK_API void Init(CJobFactory&, ILog&, IKernel&);
	NETWORK_API void Shutdown(CJobFactory&);

#ifdef __cplusplus
} 
#endif

std::auto_ptr<net::CConnectionEstablisher> g_pConnector;
void Init(CJobFactory& factory, ILog& logger, IKernel& kernel)
{
 	factory.Register<CConnect>(jobs::Job_JobId_CONNECT);
 	
 	g_pConnector.reset(new net::CConnectionEstablisher(kernel, logger));
}

void Shutdown(CJobFactory& factory)
{
	g_pConnector.reset();

	factory.Unregister(jobs::Job_JobId_CONNECT);
}


