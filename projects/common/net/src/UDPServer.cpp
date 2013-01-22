#include "UDPServer.h"
#include "Modules.h"
#include "Exception.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>

namespace net
{

const std::size_t DEFAULT_BUFFER_SIZE = 4096 * 2;
const std::size_t CURRENT_MODULE_ID = Modules::Network;



//! UDP server implementation
//! 
//! \class CUDPServer::Impl
//!
class UDPServer::Impl : boost::noncopyable
{
	//! Service type
	typedef boost::asio::io_service								Service;

	//! Server receive endpoint
	typedef boost::asio::ip::udp::endpoint						Endpoint;

	//! Endpoint ptr
	typedef  boost::shared_ptr<Endpoint>						EndpointPtr;

	//! Socket type
	typedef boost::shared_ptr<boost::asio::ip::udp::socket>		SocketPtr;

	//! Work object
	typedef boost::asio::io_service::work						Work;

	//! Type of the boost::asio signal set pointer
	typedef boost::scoped_ptr<boost::asio::signal_set> 			SignalSetPtr;

	//! UDP client implementation
	class UDPClient : public IClient, public boost::enable_shared_from_this<UDPClient>
	{
	public:
		typedef boost::shared_ptr<UDPClient> Ptr;
		UDPClient(ILog& logger, const SocketPtr& socket, const EndpointPtr& ep)
			: m_Log(logger)
			, m_Socket(socket)
			, m_Ep(ep)
		{
			SCOPED_LOG(m_Log);
		}

	private:

		void SendCallback(const boost::system::error_code& e, BufferPtr) const
		{
			SCOPED_LOG(m_Log);
			if (e)
				LOG_ERROR("Failed to send data to client [%s]. Error: [%s]") % m_Ep->address().to_string() % e.message();
		}

		virtual void SendReply(const google::protobuf::Message& message) const override
		{
			SCOPED_LOG(m_Log);

			CHECK(message.IsInitialized());
			CHECK(message.ByteSize());

			const BufferPtr buffer(new Buffer(message.ByteSize()));
			CHECK(message.SerializeToArray(&buffer->front(), buffer->size()));

			m_Socket->async_send_to(boost::asio::buffer(*buffer), *m_Ep, boost::bind(&UDPClient::SendCallback, shared_from_this(), boost::asio::placeholders::error, buffer));
		}


	private:
		ILog& m_Log;
		SocketPtr m_Socket;
		EndpointPtr m_Ep;

	};
public:

	Impl(ILog& logger, const short port, const std::size_t threads, const Callback& callback)
		: m_Log(logger)
		, m_BufferSize(DEFAULT_BUFFER_SIZE)
		, m_Callback(callback)
		, m_Work(m_Service)
	{
		SCOPED_LOG(m_Log);
		Init(port, threads);
	}

	~Impl()
	{
		SCOPED_LOG(m_Log);
	}

	void SetBufferSize(const int size)
	{
		SCOPED_LOG(m_Log);

		LOG_TRACE("Old size: [%s], new size: [%s]") % m_BufferSize % size;

		CHECK(size, size, m_BufferSize);
		m_BufferSize = size;
	}

	void Init(const unsigned short port, const std::size_t threads)
	{
		SCOPED_LOG(m_Log);
		
		LOG_TRACE("Port: [%s], buffer: [%s]") % port % m_BufferSize;

		try 
		{
			// receive socket
			m_ReceiveSocket.reset(new boost::asio::ip::udp::socket(m_Service, Endpoint(boost::asio::ip::udp::v4(), port)));
	
			for (std::size_t i = 0; i < threads; ++i)
			{
				m_Pool.create_thread(boost::bind(&boost::asio::io_service::run, &m_Service));
				StartReceiving(m_ReceiveSocket);
			}

			// Creating signal set
			m_Signals.reset(new boost::asio::signal_set(m_Service));

			m_Signals->add(SIGINT);
			m_Signals->add(SIGTERM);

#if defined(SIGQUIT)
			m_pTerminationSignals->add(SIGQUIT);
#endif // defined(SIGQUIT)

			m_Signals->async_wait(boost::bind(&Impl::Stop, this));
		}
		CATCH_PASS_EXCEPTIONS("Init failed.", port)
	}

	//! Start receiving
	void StartReceiving(const SocketPtr socket)
	{
		SCOPED_LOG(m_Log);

		TRY 
		{
			const BufferPtr buffer(new Buffer(m_BufferSize));	
			ContinueReceiving(buffer, socket);
		}
		CATCH_PASS_EXCEPTIONS("StartReceiving failed.")
	}

	//! Continue receiving
	void ContinueReceiving(const BufferPtr buffer, const SocketPtr socket)
	{
		SCOPED_LOG(m_Log);

		TRY 
		{
			const EndpointPtr ep(new Endpoint());
			socket->async_receive_from
			(
				boost::asio::buffer(*buffer), 
				*ep,
				boost::bind
				(
					&Impl::ReceiveHandle, 
					this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred,
					buffer,
					ep,
					socket
				)
			);
		}
		CATCH_PASS_EXCEPTIONS("StartReceiving failed.")
	}

	//! Receive data handle
	void ReceiveHandle
	(
		const boost::system::error_code e, 
		const std::size_t size, 
		const BufferPtr buffer, 
		const EndpointPtr ep,
		const SocketPtr socket
	)
	{
		SCOPED_LOG(m_Log);

		TRY 
		{
			if (e || !size)
			{
				LOG_ERROR("Received size: [%s], error: [%s]") % size % e.message();
				ContinueReceiving(buffer, socket);
				return;
			}

			TRY 
			{
				const UDPClient::Ptr client(new UDPClient(m_Log, socket, ep));
				buffer->resize(size);
				m_Callback(buffer, boost::ref(*client));
			}
			catch (const std::exception& e)
			{
				LOG_ERROR("Callback handle failed, error: [%s]") % e.what();
			}

			ContinueReceiving(buffer, socket);
		}
		CATCH_IGNORE_EXCEPTIONS(m_Log, "ReceiveHandle failed.")	
	}

	void Run()
	{
		m_Service.run();
	}

	void Stop()
	{
		m_Service.stop();
		m_Pool.interrupt_all();
		m_Pool.join_all();
	}

private:

	//! Logger
	ILog&				m_Log;

	//! Service
	Service				m_Service;

	//! Buffer size
	std::size_t			m_BufferSize;

	//! Server receive socket
	SocketPtr			m_ReceiveSocket;

	//! Callback
	Callback			m_Callback;

	//! Thread pool
	boost::thread_group	m_Pool;

	//! Work
	Work				m_Work;
	
	//! Signals
	SignalSetPtr		m_Signals;

};

UDPServer::UDPServer(ILog& logger, const short port, const std::size_t threads, const Callback& callback)
	: m_pImpl(new Impl(logger, port, threads, callback))
{
}

UDPServer::~UDPServer(void)
{
}

void UDPServer::SetBufferSize(const int size)
{
	m_pImpl->SetBufferSize(size);
}

void UDPServer::Run()
{
	m_pImpl->Run();
}

void UDPServer::Stop()
{
	m_pImpl->Stop();
}

} // namespace net


