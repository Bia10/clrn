#include "StdAfx.h"
#include "Event.h"

CEvent::CEvent(IKernel& kernel, const std::string& name)
	: m_Kernel(kernel)
	, m_Name(name)

{

}

CEvent::~CEvent(void)
{
}

void CEvent::Subscribe(const IJob::CallBackFn& callback, const std::string& host /*= ""*/)
{
	TRY 
	{
		IJob::TableList params;
		IJob::TablePtr param(new IJob::AutoPtr(new data::Table()));

		params.push_back(param);
	
		data::Table_Row* row = (*param)->add_rows();
		row->add_data(conv::cast<std::string>(m_Name)); // event name
		m_Kernel.ExecuteJob(jobs::Job_JobId_GET_EVENT, params, host, callback);
	}
	CATCH_PASS_EXCEPTIONS(host)
}

void CEvent::Signal(const ProtoPacketPtr packet)
{
	TRY 
	{
		// getting all packet results and pushing them to new table list
		google::protobuf::RepeatedPtrField<data::Table>& tables = *packet->mutable_job()->mutable_results();
	
		IJob::TableList params;
	
		// first table contain event params
		IJob::TablePtr eventParams(new IJob::AutoPtr(new data::Table()));
	
		data::Table_Row* row = (*eventParams)->add_rows();
		row->add_data(conv::cast<std::string>(m_Name)); // event name
	
		params.push_back(eventParams);
	
		while (tables.size())
		{
			const IJob::TablePtr table(new IJob::AutoPtr(tables.ReleaseLast()));
			params.push_back(table);
		}
	
		m_Kernel.ExecuteJob(jobs::Job_JobId_SET_EVENT, params);
	}
	CATCH_PASS_EXCEPTIONS(*packet)
}
