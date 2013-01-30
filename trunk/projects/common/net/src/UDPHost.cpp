#include "UDPHost.h"
#include "Modules.h"
#include "Exception.h"
#include "Connection.h"
#include "Conversion.h"

#include <google/protobuf/message.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

namespace net
{

const std::size_t DEFAULT_BUFFER_SIZE = 4096 * 2;

class UDPHost::Impl
{

	//! Buffer type
	typedef std::vector<char>									Buffer;

	//! Buffer pointer type
	typedef boost::shared_ptr<Buffer>							BufferPtr;

	//! Service type
	typedef boost::asio::io_service								Service;

	//! Server receive endpoint
	typedef boost::asio::ip::udp::endpoint						Endpoint;

	//! Endpoint ptr
	typedef  boost::shared_ptr<Endpoint>						EndpointPtr;

	//! Socket type
	typedef boost::asio::ip::udp::socket						Socket;

	//! Socket ptr type
	typedef boost::shared_ptr<Socket>							SocketPtr;

	//! Work object
	typedef boost::asio::io_service::work						Work;

	//! Type of the boost::asio signal set pointer
	typedef boost::scoped_ptr<boost::asio::signal_set> 			SignalSetPtr;

public:
	Impl(ILog& logger, const std::size_t threads)
		: m_Log(logger)
		, m_Threads(threads)
		, m_BufferSize(DEFAULT_BUFFER_SIZE)
		, m_Work(m_Service)
	{
		SCOPED_LOG(m_Log);
		for (std::size_t i = 0; i < m_Threads; ++i)
			m_Pool.create_thread(boost::bind(&boost::asio::io_service::run, &m_Service));	

		// Creating signal set
		m_Signals.reset(new boost::asio::signal_set(m_Service));

		m_Signals->add(SIGINT);
		m_Signals->add(SIGTERM);

#if defined(SIGQUIT)
		m_pTerminationSignals->add(SIGQUIT);
#endif // defined(SIGQUIT)

		m_Signals->async_wait(boost::bind(&Impl::Stop, this));
	}

	~Impl()
	{
		SCOPED_LOG(m_Log);
		m_Pool.join_all();
	}

	void SetBufferSize(const int size)
	{
		SCOPED_LOG(m_Log);

		LOG_TRACE("Old size: [%s], new size: [%s]") % m_BufferSize % size;

		CHECK(size, size, m_BufferSize);
		m_BufferSize = size;
	}

	void Run()
	{
		SCOPED_LOG(m_Log);
		m_Service.run();
	}

	void Stop()
	{
		SCOPED_LOG(m_Log);
		m_Pool.interrupt_all();
		m_Service.stop();
	}

	void Receive(const IHost::Callback& callback, const google::protobuf::Message& message, const short port)
	{
		SCOPED_LOG(m_Log);
		m_Message.reset(message.New());
		m_Callback = callback;

		const SocketPtr socket(new Socket(m_Service, Endpoint(boost::asio::ip::udp::v4(), port)));
		const IConnection::Ptr connection(new Connection<Socket, Endpoint>(m_Log, socket, m_BufferSize, *m_Message));

		for (std::size_t i = 0 ; i < m_Threads; ++i)
			connection->Receive(boost::bind(&Impl::ConnectionCallback, this, _1, connection, callback));
	}

	void ConnectionCallback(const google::protobuf::Message& message, const IConnection::Ptr& connection, const IHost::Callback& callback)
	{
		SCOPED_LOG(m_Log);
		callback(boost::ref(message), connection);
	}

	IConnection::Ptr Connect(const std::string& host, const short port)
	{
		SCOPED_LOG(m_Log);
		boost::asio::ip::udp::resolver resolver(m_Service);
		const boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), host, conv::cast<std::string>(port));
		const Endpoint ep = *resolver.resolve(query);

		const SocketPtr socket(new Socket(m_Service, boost::asio::ip::udp::v4()));
		const IConnection::Ptr connection(new Connection<Socket, Endpoint>(m_Log, socket, m_BufferSize, ep));
		return connection;
	}


private:

	//! Logger
	ILog&					m_Log;

	//! Threads count
	std::size_t				m_Threads;

	//! Service
	Service					m_Service;

	//! Buffer size
	std::size_t				m_BufferSize;

	//! Thread pool
	boost::thread_group		m_Pool;

	//! Work
	Work					m_Work;

	//! Signals
	SignalSetPtr			m_Signals;

	//! Message
	std::auto_ptr<google::protobuf::Message> m_Message;

	//! Callback
	IHost::Callback			m_Callback;
};

UDPHost::UDPHost(ILog& logger, const std::size_t threads) : m_Impl(new Impl(logger, threads))
{

}

UDPHost::~UDPHost()
{
	delete m_Impl;
}

void UDPHost::Run()
{
	m_Impl->Run();
}

void UDPHost::Stop()
{
	m_Impl->Stop();
}

void UDPHost::SetBufferSize(const int size)
{
	m_Impl->SetBufferSize(size);
}

IConnection::Ptr UDPHost::Connect(const std::string& host, const short port)
{
	return m_Impl->Connect(host, port);
}

void UDPHost::Receive(const IHost::Callback& callback, const google::protobuf::Message& message, const short port)
{
	m_Impl->Receive(callback, message, port);
}


} // namespace net

