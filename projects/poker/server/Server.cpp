#include "Server.h"
#include "Log.h"
#include "packet.pb.h"
#include "Exception.h"
#include "UDPHost.h"
#include "Modules.h"
#include "Parser.h"
#include "Statistics.h"

#include <iostream>

#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace srv
{

class Server::Impl
{
public:
	Impl() : m_Client(new net::UDPHost(m_Log, 3)), m_Statistics(m_Log)
	{
		m_Log.Open("1", Modules::Server, ILog::Level::Debug);
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
		std::cout << message.DebugString() << std::endl;
		Parser::Data data;

		Parser parser(m_Log, dynamic_cast<const net::Packet&>(message));
		const bool needDecision = parser.Parse();

		if (needDecision)
		{
			// react on action
		}

		// write statistics
		m_Statistics.Write(parser.GetResult());
	}

private:

	//! Logger
	Log									m_Log;

	//! UDP client
	std::auto_ptr<net::IHost>			m_Client;

	//! Server statistics
	Statistics							m_Statistics;
};

void Server::Run()
{
	m_Impl->Run();
}

Server::Server() : m_Impl(new Impl())
{

}

}