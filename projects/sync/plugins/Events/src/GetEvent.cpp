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
	SCOPED_LOG(m_Log);

	// removing subscribe
	if (!m_EventName.empty())
		CEventDispatcher::Instance().UnSubscribe(m_EventName, m_EventHash);
}

void CGetEvent::EventCallBack(const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	TRY 
	{
		TRACE_PACKET(packet);

		LOG_TRACE("Event callback, packet: [%s].") % packet->ShortDebugString(); 

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
		LOG_TRACE("Execute subscribe, packet: [%s].") % packet->ShortDebugString(); 

		// getting event params
		m_EventName = packet->job().params(0).rows(0).data(0);
		const std::string& caller = packet->job().params(0).rows(0).data(1);

		CHECK(!m_EventName.empty());

		// subscribe to event
		m_EventHash = CEventDispatcher::Instance().Subscribe(m_EventName, packet->from(), caller, boost::bind(&CGetEvent::EventCallBack, this, _1));

		// saving request packet params
		m_RequestPacketGuid = packet->guid();
		m_RequestPacketHost = packet->from();

		TRACE_PACKET(packet, m_EventHash, m_EventName, m_RequestPacketHost, m_RequestPacketGuid);

		// add this job to the list of waiting
		m_TimeOut = std::numeric_limits<std::size_t>::max();
		m_Kernel.AddToWaiting(shared_from_this(), m_RequestPacketHost);
	}
	CATCH_PASS_EXCEPTIONS("CGetEvent::Execute failed.")
}

void CGetEvent::HandleReply(const ProtoPacketPtr packet)
{
	SCOPED_LOG(m_Log);

	TRACE_PACKET(packet, m_RequestPacketHost);

	if (packet)
	{
		LOG_TRACE("Handling event reply, packet: [%s].") % packet->ShortDebugString(); 
		m_TimeOut = std::numeric_limits<std::size_t>::max();
		m_Kernel.AddToWaiting(shared_from_this(), m_RequestPacketHost);
	}

	if (!m_CallBackFn)
		return;

	TRY 
	{
		m_CallBackFn(packet);
	}
	CATCH_IGNORE_EXCEPTIONS(m_Log, "Event callback failed.")
}
