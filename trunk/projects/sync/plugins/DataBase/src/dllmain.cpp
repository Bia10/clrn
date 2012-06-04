#include "stdafx.h"

#include "ExecuteScript.h"
#include "ExecuteProcedure.h"

#ifdef __cplusplus
extern "C" 
{
#endif
 
	DATABASE_API void Init(CJobFactory&, ILog&, IKernel&);
	DATABASE_API void Shutdown(CJobFactory&);

#ifdef __cplusplus
} 
#endif

void Init(CJobFactory& factory, ILog& logger, IKernel& kernel)
{

	TRY 
	{
		DataBase::Create(logger, kernel.DbPath().c_str());
	
		factory.Register<CExecuteScript>(jobs::Job_JobId_EXEC_SCRIPT); 
		factory.Register<CExecuteProcedure>(jobs::Job_JobId_EXEC_PROCEDURE); 

		return;
	} 
	CATCH_IGNORE_EXCEPTIONS(logger, "Init failed.")

	::abort();
}

void Shutdown(CJobFactory& factory)
{
	DataBase::Shutdown();
	factory.Unregister(jobs::Job_JobId_EXEC_SCRIPT);
	factory.Unregister(jobs::Job_JobId_EXEC_PROCEDURE);
}


