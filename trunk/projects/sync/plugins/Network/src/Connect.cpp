#include "stdafx.h"
#include "Connect.h"
#include "HostController.h"

CConnect::CConnect(IKernel& kernel, ILog& logger)
	: CBaseJob(kernel, logger)
{
	m_Id = jobs::Job_JobId_PING_HOST;
}

void CConnect::Invoke(const TableList& params, const std::string& host)
{
	TRY 
	{
		// getting ping timeout
		CHECK(1 == params.size());
	
		const data::Table& table = **params.front();
	
		m_TimeOut = conv::cast<std::size_t>(table.rows(0).data(0));
		const std::string& endpoint = table.rows(0).data(1);
	
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

		// delimiter
		const std::string::size_type delimiter = endpoint.find(':');

		CHECK(std::string::npos != delimiter, endpoint);

		const std::string ip = endpoint.substr(0, delimiter);
		const std::string port = endpoint.substr(delimiter + 1);

		// this job directly sends to endpoint
		m_Kernel.Send(host, ip, port, request);
	}
	CATCH_PASS_EXCEPTIONS(host)
}

void CConnect::Execute(const ProtoPacketPtr packet)
{
	CHECK(packet);

	TRY 
	{
		CHostController::Instance().OnRemotePingReceived(packet);

		const std::string& host = packet->from();
		packet->set_type(packets::Packet_PacketType_REPLY);

		m_Kernel.Send(host, packet);
	}
	CATCH_PASS_EXCEPTIONS("CBaseJob::Execute failed.", *packet)
}