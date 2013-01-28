#include "Server.h"
#include "Log.h"
#include "packet.pb.h"
#include "Exception.h"
#include "UDPHost.h"
#include "Modules.h"
#include "Parser.h"

#include <iostream>

#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace srv
{

class Server::Impl
{
public:
	Impl() 
		: m_Client(new net::UDPHost(m_Log, 1))
		, m_Parser(m_Log)
	{
		m_Log.Open("1", Modules::Network, ILog::Level::Debug);
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
		const bool needDecision = m_Parser.Parse(dynamic_cast<const net::Packet&>(message), data);

		// write statistics

		if (needDecision)
		{
			// react on action
		}
	}

private:

	//! UDP client
	std::auto_ptr<net::IHost>			m_Client;

	//! Logger
	Log									m_Log;

	//! Parser
	Parser								m_Parser;
};

void Server::Run()
{
	m_Impl->Run();
}

Server::Server() : m_Impl(new Impl())
{

}

}