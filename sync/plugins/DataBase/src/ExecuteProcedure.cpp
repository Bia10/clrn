#include "stdafx.h"
#include "ExecuteProcedure.h"


CExecuteProcedure::CExecuteProcedure(IKernel& kernel, ILog& logger)
	: CBaseJob(kernel, logger)
{
	m_Id = jobs::Job_JobId_EXEC_PROCEDURE;
	m_TimeOut = m_Kernel.Settings().JobTimeout();
}

void CExecuteProcedure::Execute(const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	CHECK(packet);

	TRACE_PACKET(packet);

	TRY 
	{
		// first line: 0 - procedure id
		// second line: param names
		// next lines: param values

		CHECK(packet->job().params(0).rows_size() > 2);

		const data::Table& params = packet->job().params(0);

		const data::Table_Row& firstLine = params.rows(0);
		const data::Table_Row& secondLine = params.rows(1);

		const CProcedure::Id::Enum_t id = static_cast<CProcedure::Id::Enum_t>(conv::cast<int>(firstLine.data(0)));
	
		for (int row = 2; row < params.rows_size(); ++row)
		{
			CProcedure::ParamsMap paramsMap;
			for (int paramIndex = 0 ; paramIndex < secondLine.data_size(); ++paramIndex)
			{
				paramsMap.insert(std::make_pair(secondLine.data(paramIndex), params.rows(row).data(paramIndex)));
			}

			data::Table* resultTable = packet->mutable_job()->add_results();

			CProcedureExecutor::Instance().Execute(id, paramsMap, *resultTable);
		}

		CBaseJob::Execute(packet);
	}
	CATCH_PASS_EXCEPTIONS("CExecuteScript::Execute failed.", *packet)
}
