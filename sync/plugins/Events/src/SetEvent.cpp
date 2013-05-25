#include "stdafx.h"
#include "SetEvent.h"
#include "EventDispatcher.h"

CSetEvent::CSetEvent(IKernel& kernel, ILog& logger)
	: CBaseJob(kernel, logger)
{
	m_Id = jobs::Job_JobId_SET_EVENT;
	m_TimeOut = 0;
}

void CSetEvent::Invoke(const TableList& params, const std::string& /*host*/)
{
	SCOPED_LOG(m_Log)

	TRY 
	{
		CHECK(params.size() > 1, params.size());

		// getting event params
		const data::Table& eventParams = **params.front();
		const std::string& name = eventParams.rows(0).data(0);

		// signal this event
		const ProtoPacketPtr resultPacket(new packets::Packet());
		TRACE_PACKET(resultPacket);
		
		google::protobuf::RepeatedPtrField<data::Table>& packetTables = *resultPacket->mutable_job()->mutable_results();

		TableList::const_iterator it = params.begin();
		const TableList::const_iterator itEnd = params.end();
		(*it)->release();
		++it;
		for (; it != itEnd; ++it)
		{
			packetTables.AddAllocated((*it)->release());
		}

		// signal event
		CEventDispatcher::Instance().Signal(name, resultPacket);

		// invoke callback if exists
		if (m_CallBackFn)
			m_CallBackFn(resultPacket);
	}
	CATCH_PASS_EXCEPTIONS("CSetEvent::Invoke failed.")
}

void CSetEvent::Execute(const ProtoPacketPtr /*packet*/)
{

}