#include "stdafx.h"
#include "Connect.h"
#include "HostController.h"

CConnect::CConnect(IKernel& kernel, ILog& logger)
	: CBaseJob(kernel, logger)
{
	m_Id = jobs::Job_JobId_CONNECT;
}

void CConnect::Execute(const ProtoPacketPtr packet)
{
	CHECK(packet);

	TRY 
	{
		// this job executes when some host want to connect to other host.
		// we will add host_map data according to this job params.
		const std::string& targetHost = packet->job().params(0).rows(0).data(0);

		// host which want to connect
		const std::string& host = packet->from();

		// invoker host endpoint
		const std::string& ep = packet->ep();

		std::vector<std::string> IPAndPort;
		boost::algorithm::split(IPAndPort, ep, boost::algorithm::is_any_of(":"));


		// insert data to host map
		CProcedure proc(m_Kernel);
		
		CProcedure::ParamsMap params;

// 		const std::string& from		= GetParam(params, "from");
// 		const std::string& to		= GetParam(params, "to");
// 		const std::string& ip		= GetParam(params, "ip");
// 		const std::string& port		= GetParam(params, "port");	
// 		const std::string& status	= GetParam(params, "status");	
// 		const std::string& ping		= GetParam(params, "ping");	

		proc.Execute(CProcedure::Id::HostMapCreate, params, IJob::CallBackFn());
	}
	CATCH_PASS_EXCEPTIONS("CBaseJob::Execute failed.", *packet)
}