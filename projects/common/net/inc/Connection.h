#ifndef Connection_h__
#define Connection_h__

#include "IConnection.h"
#include "ILog.h"
#include "Exception.h"
#include "Modules.h"

#include <google/protobuf/message.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>

namespace net
{
const std::size_t CURRENT_MODULE_ID = Modules::Network;

template<typename Socket, typename Endpoint>
class Connection : public IConnection, public boost::enable_shared_from_this<Connection<Socket, Endpoint> >
{
	typedef boost::shared_ptr<Socket> SocketPtr;
	typedef std::vector<char> Buffer;
	typedef boost::shared_ptr<Buffer> BufferPtr;
	typedef boost::asio::ip::udp::endpoint Endpoint;
	typedef  boost::shared_ptr<Endpoint> EndpointPtr;
public:

	Connection(ILog& logger, const SocketPtr& socket, std::size_t bufferSize, const google::protobuf::Message& message) 
		: m_Log(logger)
		, m_Socket(socket)
		, m_BufferSize(bufferSize)
		, m_Message(message.New())
	{
		SCOPED_LOG(m_Log);
	}

	Connection(ILog& logger, const SocketPtr& socket, std::size_t bufferSize, const Endpoint& ep) 
		: m_Log(logger)
		, m_Endpoint(ep)
		, m_Socket(socket)
		, m_BufferSize(bufferSize)
	{
		SCOPED_LOG(m_Log);
	}

	virtual void Send(const google::protobuf::Message& message) override
	{
		SCOPED_LOG(m_Log);
		if (!m_Message.get())
			m_Message.reset(message.New());

		const BufferPtr buffer(new Buffer(message.ByteSize()));
		CHECK(message.IsInitialized(), "Message is not initialized", message.ShortDebugString());
		CHECK(message.SerializeToArray(&buffer->front(), buffer->size()), message.ShortDebugString());

		m_Socket->async_send_to(boost::asio::buffer(*buffer), m_Endpoint, boost::bind(&Connection::SendCallback, this, _1, buffer));
	}

	virtual void Receive(const IConnection::Callback& callback) override
	{
		SCOPED_LOG(m_Log);
		const BufferPtr buffer(new Buffer(m_BufferSize));
		ReceiveInBuffer(callback, buffer);
	}

private:

	void ReceiveInBuffer(const IConnection::Callback& callback, const BufferPtr buffer)
	{
		SCOPED_LOG(m_Log);
		m_Socket->async_receive_from
		(
			boost::asio::buffer(*buffer), 
			m_Endpoint,
			boost::bind
			(
				&Connection::ReceiveCallback, 
				shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				buffer,
				callback
			)
		);
	}

	void SendCallback(const boost::system::error_code& e, BufferPtr)
	{
		SCOPED_LOG(m_Log);
		if (e)
			LOG_ERROR("Failed to send data to server. Error: [%s]") % e.message();
	}

	void ReceiveCallback(const boost::system::error_code e, const std::size_t size, const BufferPtr& buffer, const IConnection::Callback& callback)
	{
		SCOPED_LOG(m_Log);

		TRY 
		{
			if (e || !size)
			{
				LOG_ERROR("Received size: [%s], error: [%s]") % size % e.message();
				ReceiveInBuffer(callback, buffer);
				return;
			}

			TRY 
			{
				const std::auto_ptr<google::protobuf::Message> mesaage(m_Message->New());
				CHECK(mesaage->ParseFromArray(&buffer->front(), size));
				callback(*mesaage);
			}
			catch (const std::exception& e)
			{
				LOG_ERROR("Callback handle failed, error: [%s]") % e.what();
			}

			ReceiveInBuffer(callback, buffer);
		}
		CATCH_IGNORE_EXCEPTIONS(m_Log, "ReceiveHandle failed.")	
	}

private:
	ILog& m_Log;
	Endpoint m_Endpoint;
	const SocketPtr m_Socket;
	const std::size_t m_BufferSize;
	std::auto_ptr<google::protobuf::Message> m_Message;
};

} // namespace net


#endif // Connection_h__
