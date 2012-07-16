#include "stdafx.h"
#include "Connect.h"
#include "UDPHost.h"

CConnect::CConnect(IKernel& kernel, ILog& logger)
	: CBaseJob(kernel, logger)
{
	m_Id = jobs::Job_JobId_CONNECT;
}

void CConnect::Invoke(const TableList& params, const std::string& host)
{
	SCOPED_LOG(m_Log);

	TRY 
	{
		// generating packet
		ProtoPacketPtr request(new packets::Packet());
		request->set_type(packets::Packet_PacketType_CONNECT);	

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

		m_Kernel.Send(host, request);
	}
	CATCH_PASS_EXCEPTIONS("CConnect::Invoke failed.", host)
}

void CConnect::Execute(const ProtoPacketPtr packet)
{
	CHECK(packet);

	TRY 
	{
		// this job executes when some host want to connect to other host.
		// we will add host_map data according to this job params.

		// from		- guid of the host which want to connect
		// to		- target host(host unreachable from 'from' host)
		// ip		- ip of the NAT endpoint(ip from received packet)
		// port		- port of the NAT endpoint(port from received packet)
		// status	- 2 (CUDPHost::Status::SessionRequested)
		// ping		- 0

		// target 
		const std::string& to = packet->job().params(0).rows(0).data(0);

		// host which want to connect
		const std::string& from = packet->from();

		// invoker host endpoint
		const std::string& ep = packet->ep();

		std::vector<std::string> ipAndPort;
		boost::algorithm::split(ipAndPort, ep, boost::algorithm::is_any_of(":"));

		CHECK(2 == ipAndPort.size());

		// insert data to host map
		CProcedure proc(m_Kernel);
		
		CProcedure::ParamsMap params;

		params["from"]		= from;
		params["to"]		= to;
		params["ip"]		= ipAndPort.front();
		params["port"]		= ipAndPort.back();
		params["status"]	= conv::cast<std::string>(net::CUDPHost::Status::SessionRequested);
		params["ping"]		= conv::cast<std::string>(std::numeric_limits<std::size_t>::max());

		proc.Execute(CProcedure::Id::HostMapCreate, params, IJob::CallBackFn());
	}
	CATCH_PASS_EXCEPTIONS("CBaseJob::Execute failed.", *packet)
}