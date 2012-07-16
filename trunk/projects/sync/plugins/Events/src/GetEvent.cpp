#include "stdafx.h"
#include "GetEvent.h"
#include "EventDispatcher.h"
#include "GUID.h"

CGetEvent::CGetEvent(IKernel& kernel, ILog& logger)
	: CBaseJob(kernel, logger)
{
	m_Id = jobs::Job_JobId_GET_EVENT;
	m_TimeOut = std::numeric_limits<std::size_t>::max();
}

CGetEvent::~CGetEvent()
{
	// removing subscribe
	//CEventDispatcher::Instance().
}

void CGetEvent::EventCallBack(const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	TRY 
	{
		// received packet with job result, send it to the subscriber
		packet->set_type(packets::Packet_PacketType_REPLY);	

		// packet GUID
		packet->set_guid(m_RequestPacketGuid);
		packet->set_timeout(0);

		m_Kernel.Send(m_RequestPacketHost, packet);
	}
	CATCH_PASS_EXCEPTIONS("CGetEvent::EventCallBack failed.")
}

void CGetEvent::Execute(const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	TRY 
	{
		// getting event params
		const std::string& name = packet->job().params(0).rows(0).data(0);

		// subscribe to event
		CEventDispatcher::Instance().Subscribe(name, boost::bind(&CGetEvent::EventCallBack, this, _1));

		// saving request packet params
		m_RequestPacketGuid = packet->guid();
		m_RequestPacketHost = packet->from();

		// add this job to the list of waiting
		m_Kernel.AddToWaiting(shared_from_this(), m_RequestPacketHost);
	}
	CATCH_PASS_EXCEPTIONS("CGetEvent::Execute failed.")
}

void CGetEvent::HandleReply(const ProtoPacketPtr packet)
{
	if (packet)
		m_Kernel.AddToWaiting(shared_from_this(), m_RequestPacketHost);

	if (!m_CallBackFn)
		return;

	TRY 
	{
		m_CallBackFn(packet);
	}
	CATCH_IGNORE_EXCEPTIONS(m_Log, "Event callback failed.")
}
