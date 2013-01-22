#include "Server.h"
#include "UDPServer.h"
#include "Log.h"
#include "packet.pb.h"
#include "Exception.h"

#include <iostream>

#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace srv
{

class Server::Impl
{
public:
	Impl()
	{
		m_Server.reset(new net::UDPServer(m_Log, 5000, 3, boost::bind(&Impl::HandleRequest, this, _1, _2)));
	}

	void Run()
	{
		m_Server->Run();
	}

private:

	//! Client handle
	void HandleRequest(const net::UDPServer::BufferPtr& buffer, const net::UDPServer::IClient& client)
	{
		net::Packet packet;
		CHECK(packet.ParseFromArray(&buffer->front(), buffer->size()));

		std::cout << packet.DebugString() << std::endl;
	}

private:

	//! UDP server
	std::auto_ptr<net::UDPServer>		m_Server;

	//! Logger
	Log									m_Log;
};

void Server::Run()
{
	m_Impl->Run();
}

Server::Server() : m_Impl(new Impl())
{

}

}