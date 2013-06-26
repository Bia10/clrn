#include "Server.h"
#include "Log.h"
#include "packet.pb.h"
#include "Exception.h"
#include "UDPHost.h"
#include "Modules.h"
#include "MongoStatistics.h"
#include "DecisionMaker.h"
#include "Evaluator.h"
#include "../neuro/DatabaseReader.h"
#include "TableLogic.h"

#include <iostream>
#include <atomic>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

namespace srv
{

const int CURRENT_MODULE_ID = Modules::Server;

class Server::Impl
{
    //! Statistics to thread id map
    typedef std::map<unsigned, boost::shared_ptr<IStatistics>> StatisticsMap;

public:
	Impl() 
		: m_Client(new net::UDPHost(m_Log, cfg::THREADS_COUNT - 1))
        , m_Statistics()
		, m_RequestsCount(0)
		, m_Network(m_Log, cfg::NETWORK_FILE_NAME)
	{
        m_Log.Open("logs/server.txt", Modules::Server, ILog::Level::Trace);
        m_Log.Open("1", Modules::Server, ILog::Level::Trace);
        m_Log.Open("logs/logic.txt", Modules::TableLogic, ILog::Level::Trace);
        m_Log.Open("logs/database.txt", Modules::DataBase, ILog::Level::Trace);

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

#ifdef _DEBUG
		std::string request;
#endif
		try
		{
#ifdef _DEBUG
			request = message.DebugString();
			assert(!request.empty());
#endif
			LOG_DEBUG("Request: [%s]") % message.DebugString();

            IStatistics* stats = 0;
            {
                boost::mutex::scoped_lock lock(m_StatsMutex);
                StatisticsMap::iterator it = m_Statistics.find(GetCurrentThreadId());
                if (it == m_Statistics.end())
                    it = m_Statistics.insert(std::make_pair(GetCurrentThreadId(), boost::shared_ptr<IStatistics>(new MongoStatistics(m_Log)))).first;
                stats = it->second.get();
            }

			DecisionMaker decisionMaker(m_Log, m_Evaluator, *stats, m_Network, *connection);
            pcmn::TableLogic logic(m_Log, decisionMaker, m_Evaluator);
            logic.Parse(static_cast<const net::Packet&>(message));

			LOG_WARNING("Requests processed: [%s]") % ++m_RequestsCount;
		}
		catch (const std::exception& e)
		{
			std::ostringstream oss;
			oss << "Error: " << e.what() << std::endl
				<< "Packet: " << message.DebugString();

			net::Reply reply;
			reply.set_error(oss.str());

			LOG_ERROR("%s") % oss.str();
			connection->Send(reply);
		}
	}

private:

	//! Logger
	Log									m_Log;

	//! UDP client
	std::auto_ptr<net::IHost>			m_Client;

	//! Server statistics
	StatisticsMap						m_Statistics;

    //! Statistics mutex
    boost::mutex                        m_StatsMutex;

	//! Requests count
	std::atomic<std::size_t>			m_RequestsCount;

	//! Evaluator
	pcmn::Evaluator						m_Evaluator;

	//! Neuro network
	neuro::DatabaseReader				m_Network;
};

void Server::Run()
{
	m_Impl->Run();
}

Server::Server() : m_Impl(new Impl())
{

}

Server::~Server()
{
	delete m_Impl;
}

}