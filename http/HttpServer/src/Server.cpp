/*
 * Server.cpp
 *
 *  Created on: Nov 5, 2011
 *      Author: root
 */

#include "Server.h"
#include "Connection.h"
#include "Exception.h"
#include "ServerLogger.h"
#include "ServerSettings.h"
#include "DefaultRequestExecutor.h"

#include <google/profiler.h>

#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/asio/placeholders.hpp>

namespace srv
{

CServer::CServer(cmn::ILog& Logger, const _tstring& sHost, const std::size_t& nPort, const std::size_t& nThreads) :
		m_bIsInited(false),
		m_sHost(sHost),
		m_nPort(nPort),
		m_nThreads(nThreads),
		m_nCurrentServiceIndex(0),
		m_Log(Logger)
{
	SCOPED_LOG_FUNCTION
}

CServer::~CServer()
{
	SCOPED_LOG_FUNCTION

	if (m_pPlugginLoader)
		m_pPlugginLoader->Free();
}


void CServer::Init()
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		if (m_nThreads <= 0)
			THROW_EXCEPTION("Number of threads must be greater than zero.");

		if (m_sHost.empty())
			THROW_EXCEPTION("Host must be non empty string.");

		if (m_nPort <= 0)
			THROW_EXCEPTION("Working port must be greater than zero.");

		// Filling up asio service pool and work pool
		for (std::size_t i = 0; i < m_nThreads; ++i)
		{
			const TServicePtr pService(new boost::asio::io_service());
			m_vecIOServicePool.push_back(pService);

			const TWorkPtr pWork(new boost::asio::io_service::work(*pService));
			m_vecWork.push_back(pWork);
		}

		// Refference to asio service object needed for initialization
		boost::asio::io_service& Service = *m_vecIOServicePool[0];

		// Creating acceptor and signal set
		m_pAcceptor.reset(new boost::asio::ip::tcp::acceptor(Service));
		m_pTerminationSignals.reset(new boost::asio::signal_set(Service));

		boost::asio::ip::tcp::resolver Resolver(Service);

		const boost::asio::ip::tcp::resolver::query AddressQuery(m_sHost, boost::lexical_cast< _tstring >(m_nPort));
		const boost::asio::ip::tcp::endpoint& EndPoint = *Resolver.resolve(AddressQuery);

		m_pAcceptor->open(EndPoint.protocol());
		m_pAcceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		m_pAcceptor->bind(EndPoint);
		m_pAcceptor->listen();

		// Register to handle the signals that indicate when the server should exit.
		// It is safe to register for the same signal multiple times in a program,
		// provided all registration for the specified signal is made through Asio.
		m_pTerminationSignals->add(SIGINT);
		m_pTerminationSignals->add(SIGTERM);

		#if defined(SIGQUIT)
		m_pTerminationSignals->add(SIGQUIT);
		#endif // defined(SIGQUIT)

		m_pTerminationSignals->async_wait(boost::bind(&CServer::Stop, this));

		LoadPlugins();

		m_bIsInited = true;
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

void CServer::LoadPlugins()
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		m_ExecutorsFactory.Add< CDefaultRequestExecutor >(0);

		// Setting up pluggin loader
		const _tstring sPluginPath = cmn::Settings::Instance().GetPluginsPath();
		const CPluginLoader::TPlugins vecPlugins = cmn::Settings::Instance().GetPluginsList();

		m_pPlugginLoader.reset(new CPluginLoader(m_ExecutorsFactory, sPluginPath, vecPlugins));

		m_pPlugginLoader->Load();
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}


void CServer::Run()
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		assert(m_bIsInited);

		// Creating thread group object
		m_pThreadPool.reset(new boost::thread_group());

		for (const auto& pService : m_vecIOServicePool)
		{
			// Starting accepting
			StartAccept();

			m_pThreadPool->create_thread(boost::bind(&boost::asio::io_service::run, pService));
		}

		m_pThreadPool->join_all();
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}


boost::asio::io_service & CServer::GetIOService()
{
	if (m_nCurrentServiceIndex == m_vecIOServicePool.size())
		m_nCurrentServiceIndex = 0;

	return *m_vecIOServicePool[m_nCurrentServiceIndex++];
}

void CServer::AcceptHandler(const boost::system::error_code & e, const TConnectionPointer pClientConnection)
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		pClientConnection->StartRead();

		StartAccept();
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

void CServer::Stop()
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		cmn::Logger::Instance() << "Terminating...";
		for (const auto& pService : m_vecIOServicePool)
		{
			pService->stop();
		}


	/*	ProfilerFlush();
		ProfilerStop();*/
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

void CServer::StartAccept()
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		TConnectionPointer pConnection(new CConnection(GetIOService(), m_ExecutorsFactory));

		m_pAcceptor->async_accept(pConnection->GetSocket(), boost::bind(&CServer::AcceptHandler, this, boost::asio::placeholders::error, pConnection));
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

} /* namespace srv */
