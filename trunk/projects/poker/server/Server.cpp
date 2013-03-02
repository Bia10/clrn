#include "Server.h"
#include "Log.h"
#include "packet.pb.h"
#include "Exception.h"
#include "UDPHost.h"
#include "Modules.h"
#include "Parser.h"
#include "Statistics.h"
#include "DecisionMaker.h"
#include "Evaluator.h"
#include "../neuro/NeuroNetwork.h"

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
		: m_Client(new net::UDPHost(m_Log, cfg::THREADS_COUNT - 1))
		, m_Statistics(m_Log)
		, m_RequestsCount(0)
		, m_Network(cfg::NETWORK_FILE_NAME)
	{
		m_Log.Open("1", Modules::Server, ILog::Level::Trace);
		m_Log.Open("1", Modules::TableLogic, ILog::Level::Trace);
		m_Log.Open("1", Modules::DataBase, ILog::Level::Trace);

		m_Client->Receive(boost::bind(&Impl::HandleRequest, this, _1, _2), net::Packet(), cfg::DEFAULT_PORT);
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
		LOG_DEBUG("Request: [%s]") % message.DebugString();

		DecisionMaker decisionMaker(m_Log, m_Evaluator, m_Statistics, m_Network, *connection);

		Parser parser(m_Log, m_Evaluator, dynamic_cast<const net::Packet&>(message), decisionMaker);

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

	//! Evaluator
	pcmn::Evaluator						m_Evaluator;

	//! Neuro network
	neuro::Network						m_Network;
};

void Server::Run()
{
	m_Impl->Run();
}

Server::Server() : m_Impl(new Impl())
{

}

}