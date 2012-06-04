#include "stdafx.h"
#include "BaseJob.h"
#include "GUID.h"


CBaseJob::CBaseJob(IKernel& kernel, ILog& logger)
	: m_Kernel(kernel)
	, m_Log(logger)
	, m_TimeOut(0)
	, m_GUID(CGUID::Generate())
{

}

CBaseJob::~CBaseJob(void)
{

}

void CBaseJob::Execute(const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	CHECK(packet);

	TRY 
	{
		const std::string& host = packet->from();
		packet->set_type(packets::Packet_PacketType_REPLY);

		jobs::Job& job = *packet->mutable_job();
		job.clear_params();
		
		m_Kernel.Send(host, packet);
	}
	CATCH_PASS_EXCEPTIONS("CBaseJob::Execute failed.", *packet)
}

void CBaseJob::Invoke(const TableList& params, const std::string& host)
{
	SCOPED_LOG(m_Log);

	TRY 
	{
		// generating packet
		ProtoPacketPtr request(new packets::Packet());
		request->set_type(packets::Packet_PacketType_REQUEST);	

		// packet GUID
		request->set_guid(m_GUID);
		request->set_timeout(m_TimeOut);

		jobs::Job* job = request->mutable_job();
		job->set_id(GetId());

		BOOST_FOREACH(const IJob::TablePtr& table, params)
		{
			google::protobuf::RepeatedPtrField<data::Table>& field = *job->mutable_params();
			field.AddAllocated(table->release());
		}

		if (m_TimeOut)
			m_Kernel.AddToWaiting(shared_from_this(), host);

		m_Kernel.Send(host, request);
	}
	CATCH_PASS_EXCEPTIONS("CBaseJob::Invoke failed.", host)
}

void CBaseJob::SetCallBack(const CallBackFn& callBack)
{
	m_CallBackFn = callBack;
}

void CBaseJob::HandleReply(const ProtoPacketPtr packet)
{
	if (m_CallBackFn)
		m_CallBackFn(packet);
}

jobs::Job_JobId CBaseJob::GetId() const
{
	return m_Id;
}

std::size_t CBaseJob::GetTimeout() const
{
	return m_TimeOut;
}

const std::string& CBaseJob::GetGUID() const
{
	return m_GUID;
}