#include "DataSender.h"
#include "Exception.h"
#include "Modules.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

namespace ba = boost::asio;

namespace clnt
{

const int CURRENT_MODULE_ID = Modules::Network;

class DataSender::Impl
{
public:
	Impl(ILog& logger) 
		: m_Log(logger)
		, m_Socket(m_Service)
		, m_Endpoint(ba::ip::address_v4::from_string("127.0.0.1"), 5000)
	{
		boost::thread(boost::bind(&ba::io_service::run, &m_Service));
	}

	~Impl()
	{
		m_Service.stop();
	}

	void OnGameFinished(const net::Packet& packet)
	{
		TRY 
		{
			CHECK(packet.IsInitialized());
			CHECK(packet.ByteSize());

			const BufferPtr buffer(new Buffer(packet.ByteSize()));
			packet.SerializeToArray(&buffer->front(), buffer->size());

			m_Socket.async_send_to(ba::buffer(*buffer), m_Endpoint, boost::bind(&Impl::SendCallback, this, ba::placeholders::error, buffer));
		}
		CATCH_PASS_EXCEPTIONS("Send data failed")
	}

private:

	void SendCallback(const boost::system::error_code& e, BufferPtr)
	{
		if (e)
			LOG_ERROR("Failed to send data to server. Error: [%s]") % e.message();
	}

private:
	ba::io_service m_Service;
	ILog& m_Log;
	ba::ip::udp::socket m_Socket;
	ba::ip::udp::endpoint m_Endpoint;
};

void DataSender::OnGameFinished(const net::Packet& packet)
{
	m_Impl->OnGameFinished(packet);
}

DataSender::DataSender(ILog& logger) : m_Impl(new Impl(logger))
{
	
}

}

