/*
 * Server.h
 *
 *  Created on: Nov 5, 2011
 *      Author: root
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "Types.h"
#include "ServerLogger.h"
#include "Factory.h"
#include "PluginLoader.h"

#include <vector>

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/ip/tcp.hpp>

//! Forward declarations.
namespace boost
{
	class thread;
	class thread_group;
}
namespace srv
{

//! Forward declarations.
class CConnection;


class CServer :
	private boost::noncopyable
{
private:

	//! Type of the pointer to the incoming connection
	typedef boost::shared_ptr< CConnection > 						TConnectionPointer;

	//! Type of the pointer to the boost::asio service
	typedef boost::shared_ptr< boost::asio::io_service > 			TServicePtr;

	//! Type of the IO service pool container
	typedef std::vector< TServicePtr > 								TServicePool;

	//! Type of the thread container
	typedef boost::scoped_ptr< boost::thread_group > 				TThreadPool;

	//! Type of the boost::asio acceptor pointer
	typedef boost::scoped_ptr< boost::asio::ip::tcp::acceptor > 	TAcceptorPtr;

	//! Type of the boost::asio signal set pointer
	typedef boost::scoped_ptr< boost::asio::signal_set > 			TSignalSetPtr;

	//! Type of the work IO service pointer
	typedef boost::shared_ptr< boost::asio::io_service::work > 		TWorkPtr;

	//! Type of the work IO service container
	typedef std::vector< TWorkPtr > 								TWorkPool;

	//! Type of the pointer to pluggin loader
	typedef boost::scoped_ptr< CPluginLoader >						TPlugginLoaderPtr;

public:

	//! Constructors
	CServer() = delete; //! Not implemented
	CServer(cmn::ILog& Logger, const _tstring& sHost, const std::size_t& nPort, const std::size_t& nThreads);
	~CServer();

	//! Init server
	void 								Init();

	//! Run server
	void 								Run();

	//! Stop server
	void 								Stop();

private:

	//! Accept handler
	void 								AcceptHandler(const boost::system::error_code& e, const TConnectionPointer pClientConnection);

	//! Starts new accept
	void 								StartAccept();

	//! Get refference to next IO service object
	boost::asio::io_service& 			GetIOService();

	//! Load plugins
	void 								LoadPlugins();

	//! initialization flag
	bool								m_bIsInited;

	//! Server host
	const _tstring						m_sHost;

	//! Server port
	const std::size_t 					m_nPort;

	//! Server threads count
	const std::size_t 					m_nThreads;

	//! Current IO service object index
	std::size_t							m_nCurrentServiceIndex;

	//! IO service pool
	TServicePool						m_vecIOServicePool;

	//! Acceptor
	TAcceptorPtr 						m_pAcceptor;

	//! Signal set to catch server termination signals
	TSignalSetPtr						m_pTerminationSignals;

	//! Threads pool
	TThreadPool							m_pThreadPool;

	//! Work container
	TWorkPool							m_vecWork;

	//! Logger reference
	cmn::ILog&							m_Log;

	//! Executors factory
	cmn::TExecutorsFactory				m_ExecutorsFactory;

	//! Pluggin loader
	TPlugginLoaderPtr					m_pPlugginLoader;
};

} /* namespace srv */
#endif /* SERVER_H_ */
