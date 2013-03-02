#include "Server.h"
#include "Log.h"
#include "packet.pb.h"
#include "Exception.h"
#include "UDPHost.h"
#include "Modules.h"
#include "Parser.h"
#include "Statistics.h"
#include "DecisionMaker.h"

#include <iostream>
#include <atomic>

#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace srv
{

const int CURRENT_MODULE_ID = Modules::Server;

class Server::Impl
{
public:
	Impl() 
		: m_Client(new net::UDPHost(m_Log, 3))
		, m_Statistics(m_Log)
		, m_RequestsCount(0)
	{
		m_Log.Open("1", Modules::Server, ILog::Level::Warning);
		m_Client->Receive(boost::bind(&Impl::HandleRequest, this, _1, _2), net::Packet(), 5000);
	}

	void Run()
	{
		m_Client->Run();
	}

private:

	//! Client handle
	void HandleRequest(const google::protobuf::Message& message, const net::IConnection::Ptr& connection)
	{
		SCOPED_LOG(m_Log);
		LOG_TRACE("Request: [%s]") % message.DebugString();

		pcmn::Decisionmaker decisionMaker(m_Log);

		Parser parser(m_Log, dynamic_cast<const net::Packet&>(message), decisionMaker);

		if (parser.Parse())  // write statistics, game completed
			m_Statistics.Write(parser.GetResult());

		LOG_WARNING("Requests processed: [%s]") % m_RequestsCount++;
	}

private:

	//! Logger
	Log									m_Log;

	//! UDP client
	std::auto_ptr<net::IHost>			m_Client;

	//! Server statistics
	Statistics							m_Statistics;

	//! Requests count
	std::atomic<std::size_t>			m_RequestsCount;
};

void Server::Run()
{
	m_Impl->Run();
}

Server::Server() : m_Impl(new Impl())
{

}

}