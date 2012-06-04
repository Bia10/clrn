#include "stdafx.h"
#include "SetSettings.h"


CSetSettings::CSetSettings(IKernel& kernel, ILog& logger)
	: CBaseJob(kernel, logger)
{
	m_Id = jobs::Job_JobId_SET_SETTINGS;
	m_TimeOut = 1000;
}

void CSetSettings::Execute(const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	TRY 
	{	
		CSettings settings(m_Log, DataBase::Instance(), *packet->mutable_job()->add_results());
		settings.Save();

		CBaseJob::Execute(packet);
	}
	CATCH_PASS_EXCEPTIONS("CSetSettings::Execute failed.")
}
