#include "stdafx.h"
#include "GetSettings.h"
#include "IKernel.h"
#include "Table.h"
#include "SQLiteDB.h"

CGetSettings::CGetSettings(IKernel& kernel, ILog& logger)
	: CBaseJob(kernel, logger)
{
	m_Id = jobs::Job_JobId_GET_SETTINGS;
	m_TimeOut = m_Kernel.Settings().JobTimeout();
}

void CGetSettings::Execute(const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	TRY 
	{
		TRACE_PACKET(packet);

		std::string query = "select * from settings";

		if (packet->job().params(0).rows_size())
		{
			// getting module id
			const std::string module = packet->job().params(0).rows(0).data(0);
			query += " where module = " + module;
		}

		IKernel::TablesList resultTables;

		data::Table scriptParams;
		scriptParams.add_rows()->add_data(query);
		//m_Kernel.ExecuteJob(jobs::Job_JobId_EXEC_SCRIPT, resultTables, "", &scriptParams);

		CHECK(1 == resultTables.size(), resultTables.size());

		data::Table* table = packet->mutable_job()->add_results();

		*table = *resultTables.front();

		CBaseJob::Execute(packet);

	}
	CATCH_PASS_EXCEPTIONS("CGetSettings::Execute failed.")
}