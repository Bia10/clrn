#include "stdafx.h"
#include "ExecuteScript.h"


CExecuteScript::CExecuteScript(IKernel& kernel, ILog& logger)
	: CBaseJob(kernel, logger)
{
	m_Id = jobs::Job_JobId_EXEC_SCRIPT;
	m_TimeOut = m_Kernel.Settings().JobTimeout();
}

void CExecuteScript::Execute(const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	CHECK(packet);

	TRACE_PACKET(packet);

	TRY 
	{	
		for (int i = 0 ; i < packet->job().params(0).rows_size(); ++i)
		{
			const std::string& query = packet->job().params(0).rows(i).data(0);

			data::Table* table = packet->mutable_job()->add_results();

			DataBase::Instance().Execute(query.c_str(), *table);
		}

		CBaseJob::Execute(packet);
	}
	CATCH_PASS_EXCEPTIONS("CExecuteScript::Execute failed.", *packet)
}
