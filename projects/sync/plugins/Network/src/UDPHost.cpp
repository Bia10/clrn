#include "stdafx.h"
#include "UDPHost.h"

namespace net
{

//! Host implementation
class CUDPHost::Impl : boost::noncopyable
{
	//! Buffer type
	typedef boost::shared_ptr< std::vector<char> >				BufferPtr;

	//! Buffer descriptor
	struct BufferDsc
	{
		BufferPtr					buffer;
		SocketPtr					socket;
		EndpointPtr					ep;
		boost::posix_time::ptime	time;
	};

	//! Waiting packets map type
	typedef std::map<std::string, BufferDsc> 					PacketsMap;

	//! Resend ratio
	enum														{RESEND_RATIO = 3};

	//! Delete ratio
	enum														{DELETE_RATIO = 6};

public:
	Impl
	(
		boost::asio::io_service& srvc, 
		IKernel& kernel, 
		ILog& log, 
		const std::size_t bufferSize,
		const std::size_t pingInterval,
		const std::string& remoteGuid,
		const std::string& localGuid, 
		const SocketPtr srvSocket
	)
		: m_IOService(srvc)
		, m_Kernel(kernel)
		, m_Log(log)
		, m_BufferSize(bufferSize)
		, m_PingInterval(pingInterval)
		, m_RemoteGuid(remoteGuid)
		, m_LocalGuid(localGuid)
		, m_pSrvSocket(srvSocket)
		, m_OutgoingPing(std::numeric_limits<std::size_t>::max())
		, m_IncomingPing(std::numeric_limits<std::size_t>::max())
	{

	}
	
	void Send(const ProtoPacketPtr packet)
	{
		if (m_OutgoingPing < m_IncomingPing || !m_IncomingEP)
			Send2Outgoing(packet);
		else
			Send2Incoming(packet);
	}

	void SendPingPacket(const ProtoPacketPtr packet, const std::string& ip, const std::string& port)
	{
		boost::asio::ip::udp::resolver resolver(m_IOService);
		const boost::asio::ip::udp::resolver::query addressQuery(ip, port);
		const EndpointPtr ep(new Endpoint(*resolver.resolve(addressQuery)));
		Send2Outgoing(packet, true, ep);
	}

	void SetOutgoingEP(const std::string& ip, const std::string& port, const std::size_t ping)
	{
		TRY 
		{
			boost::asio::ip::udp::resolver resolver(m_IOService);
			const boost::asio::ip::udp::resolver::query addressQuery(ip, port);
			m_OutgoingEP.reset(new Endpoint(*resolver.resolve(addressQuery)));
			m_OutgoingPing = ping;
		}
		CATCH_PASS_EXCEPTIONS(ip, port, ping)
	}

	void SetIncomingEP(const std::string& ip, const std::string& port, const std::size_t ping)
	{
		TRY 
		{
			boost::asio::ip::udp::resolver resolver(m_IOService);
			const boost::asio::ip::udp::resolver::query addressQuery(ip, port);
			m_IncomingEP.reset(new Endpoint(*resolver.resolve(addressQuery)));
			m_IncomingPing = ping;
		}
		CATCH_PASS_EXCEPTIONS(ip, port, ping)
	}

	//! Handle new packet
	void HandlePacket(const SocketPtr socket, const ProtoPacketPtr packet, const EndpointPtr client)
	{
		SCOPED_LOG(m_Log);

		CHECK(packet);

		TRY 
		{
			// checking packet type
			if (packets::Packet_PacketType_ACK == packet->type())
			{
				//DeleteWaitingPacket(packet->guid(), "delivered");
				return;
			}
		
			// getting sender endpoint
			const std::string ep = client->address().to_string() + ":" + conv::cast<std::string>(client->port());
	
			// setting up packet field
			packet->set_ep(ep);
	
			// handle packet by kernel
			m_Kernel.HandleNewPacket(packet);
	
			// sending ACK
			const ProtoPacketPtr ack(new packets::Packet());
			ack->set_from(m_LocalGuid);
			ack->set_guid(packet->guid());
			ack->set_type(packets::Packet_PacketType_ACK);
	
			const BufferPtr buffer(new std::vector<char>(ack->ByteSize()));
			ack->SerializeToArray(&buffer->front(), buffer->size());
	
			socket->async_send_to
			(
				boost::asio::buffer(*buffer), 
				*client, 
				boost::bind
				(
					&Impl::SendHandler,
					this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred,
					buffer
				)
			);	
		}
		CATCH_PASS_EXCEPTIONS(*packet)
	}

private:
	
	//! Send handler
	void SendHandler(const boost::system::error_code e, const std::size_t size, const BufferPtr /*sendBuffer*/)
	{
		if (e)
		{
			LOG_ERROR("Send error. Size: [%s], error: [%s]") % size % e.message();
		}
	}

	//! Send buffer
	void SendBuffer(const BufferPtr buffer, const std::string& guid, const EndpointPtr ep, const SocketPtr socket, bool ignoreACK)
	{
		SCOPED_LOG(m_Log);

		CHECK(buffer);

		TRY 
		{
			socket->async_send_to
			(
				boost::asio::buffer(*buffer), 
				*ep, 
				boost::bind
				(
					&Impl::SendHandler,
					this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred,
					buffer
				)
			);	

			if (ignoreACK)
				return;

			// saving packet until ACK received or timeout expired
			{
				boost::mutex::scoped_lock lock(m_PacketsMutex);
				BufferDsc& dsc = m_WaitingPackets[guid];
				dsc.buffer = buffer;
				dsc.ep = ep;
				dsc.socket = socket;
				dsc.time = boost::posix_time::microsec_clock::local_time();
			}

			// setup timer for resend this data
			m_Kernel.TimeEvent(boost::posix_time::milliseconds(m_PingInterval * RESEND_RATIO), boost::bind(&Impl::ResendPacket, this, guid), false);

			// setting up timer to delete this packet
			m_Kernel.TimeEvent(boost::posix_time::milliseconds(m_PingInterval * DELETE_RATIO), boost::bind(&Impl::DeleteWaitingPacket, this, guid, "timed out"), false);
		}
		CATCH_PASS_EXCEPTIONS("SendBuffer failed.", guid, ignoreACK)
	}

	//! Serialize buffer
	BufferPtr Serialize(const ProtoPacketPtr packet)
	{
		// setting up guid
		packet->set_from(m_LocalGuid);

		// sending
		const BufferPtr buffer(new std::vector<char>(packet->ByteSize()));
		packet->SerializeToArray(&buffer->front(), buffer->size());

		return buffer;
	}

	//! Start receiving
	void StartReceiving(const SocketPtr socket)
	{
		SCOPED_LOG(m_Log);

		TRY 
		{
			const BufferPtr buffer(new std::vector<char>());	
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
			const std::size_t received = buffer->size();
			buffer->resize(received + m_BufferSize);

			const EndpointPtr ep(new Endpoint);
			socket->async_receive_from
			(
				boost::asio::buffer(&buffer->at(received), buffer->size() - received), 
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
		std::size_t size, 
		const BufferPtr buffer, 
		const EndpointPtr client,
		const SocketPtr socket
	)
	{
		SCOPED_LOG(m_Log);

		TRY 
		{
			if (socket == m_pSrvSocket)
				StartReceiving(socket);

			if (e)
			{
				LOG_ERROR("Received: [%s], size: [%s], error: [%s]") % &buffer->front() % size % e.message();
				return;
			}

			if (!size)
				return;

			if (buffer->size() > m_BufferSize)
				size += m_BufferSize;

			const ProtoPacketPtr packet(new packets::Packet());
			if (packet->ParseFromArray(&buffer->front(), size))
			{
				LOG_TRACE("Received from: [%s], socket: [%s], ep: [%s]:[%s]") 
					% packet->from()
					% (socket == m_pSrvSocket ? "IN" : "OUT")
					% client->address().to_string() 
					% client->port();

				HandlePacket(socket, packet, client);
			}
			else 
			if (size == m_BufferSize)
				ContinueReceiving(buffer, socket);
		}
		CATCH_IGNORE_EXCEPTIONS(m_Log, "ReceiveHandle failed.")	
	}

	//! Send to outgoing channel
	void Send2Outgoing(const ProtoPacketPtr packet, bool ignoreACK = false, const EndpointPtr ep = EndpointPtr())
	{
		const EndpointPtr endpoint = ep ? ep : m_OutgoingEP;
		CHECK(endpoint);

		LOG_TRACE("Sending to: [%s], as [OUT], ep:[%s]:[%s], data: [%s]") 
			% m_RemoteGuid 
			% endpoint->address().to_string() 
			% endpoint->port() 
			% packet->ShortDebugString();

		// create socket if not exists and send data
		if (!m_pOutgoingSocket)
		{
			m_pOutgoingSocket.reset(new boost::asio::ip::udp::socket(m_IOService));
			m_pOutgoingSocket->open(boost::asio::ip::udp::v4());
		}

		SendBuffer(Serialize(packet), packet->guid(), endpoint, m_pOutgoingSocket, ignoreACK);

		// starting to receive from this socket
		StartReceiving(m_pOutgoingSocket);
	}

	//! Send to incoming channel
	void Send2Incoming(const ProtoPacketPtr packet)
	{
		CHECK(m_IncomingEP);

		LOG_TRACE("Sending to: [%s], as [IN], ep:[%s]:[%s], data: [%s]") 
			% m_RemoteGuid 
			% m_IncomingEP->address().to_string() 
			% m_IncomingEP->port() 
			% packet->ShortDebugString();

		// send through server incoming socket
		SendBuffer(Serialize(packet), packet->guid(), m_IncomingEP, m_pSrvSocket, false);
	}

	//! Delete packet by guid
	void DeleteWaitingPacket(const std::string& packet, const std::string& reason)
	{
		boost::mutex::scoped_lock lock(m_PacketsMutex);

		if (!m_WaitingPackets.count(packet))
			return;

		const boost::posix_time::time_duration td = boost::posix_time::microsec_clock::local_time() - 
			m_WaitingPackets[packet].time;

		LOG_TRACE("Deleting packet: [%s], to: [%s], reason: [%s], millisec: [%s]") 
			% packet 
			% m_LocalGuid 
			% reason 
			% static_cast<std::size_t>(td.total_milliseconds());

		m_WaitingPackets.erase(packet);
	}

	//! Resend packet
	void ResendPacket(const std::string& packet)
	{
		TRY 
		{
			boost::mutex::scoped_lock lock(m_PacketsMutex);

			if (!m_WaitingPackets.count(packet))
				return;

			const BufferDsc& dsc = m_WaitingPackets[packet];

			LOG_TRACE("Resending packet: [%s], to: [%s]") % packet % m_LocalGuid;

			SendBuffer(dsc.buffer, packet, dsc.ep, dsc.socket, true);

			// setup timer for resend this data
			m_Kernel.TimeEvent(boost::posix_time::milliseconds(m_PingInterval * RESEND_RATIO), boost::bind(&Impl::ResendPacket, this, packet), false);
		}
		CATCH_PASS_EXCEPTIONS(m_LocalGuid, packet)
	}

	//! Asio service
	boost::asio::io_service&			m_IOService;;

	//! Kernel interface
	IKernel&							m_Kernel;

	//! Logger
	ILog&								m_Log;

	//! Buffer size
	const std::size_t					m_BufferSize;

	//! Ping interval
	const std::size_t					m_PingInterval;

	//! Remote host guid
	const std::string					m_RemoteGuid;

	//! Local host guid
	const std::string					m_LocalGuid;

	//! Server socket
	const SocketPtr						m_pSrvSocket;

	//! Outgoing endpoint
	EndpointPtr							m_OutgoingEP;

	//! Incoming endpoint
	EndpointPtr							m_IncomingEP;

	//! Outgoing ping
	std::size_t							m_OutgoingPing;

	//! Incoming ping
	std::size_t							m_IncomingPing;

	//! Outgoing socket
	SocketPtr							m_pOutgoingSocket;

	//! Waiting for ACK packets
	PacketsMap							m_WaitingPackets;

	//! Mutex for waiting packets
	boost::mutex						m_PacketsMutex;
};

CUDPHost::CUDPHost
(
	boost::asio::io_service& srvc, 
	IKernel& kernel, 
	ILog& log, 
	const std::size_t bufferSize,
	const std::size_t pingInterval,
	const std::string& remoteGuid,
	const std::string& localGuid, 
	const SocketPtr srvSocket
)
	: m_pImpl(new Impl(srvc, kernel, log, bufferSize, pingInterval, remoteGuid, localGuid, srvSocket))
{

}


CUDPHost::~CUDPHost(void)
{
}

void CUDPHost::Send(const ProtoPacketPtr packet)
{
	m_pImpl->Send(packet);
}

void CUDPHost::SetOutgoingEP(const std::string& ip, const std::string& port, const std::size_t ping)
{
	m_pImpl->SetOutgoingEP(ip, port, ping);
}

void CUDPHost::SetIncomingEP(const std::string& ip, const std::string& port, const std::size_t ping)
{
	m_pImpl->SetIncomingEP(ip, port, ping);
}

void CUDPHost::HandlePacket(const SocketPtr socket, const ProtoPacketPtr packet, const EndpointPtr client)
{
	m_pImpl->HandlePacket(socket, packet, client);
}

void CUDPHost::SendPingPacket(const ProtoPacketPtr packet, const std::string& ip, const std::string& port)
{
	m_pImpl->SendPingPacket(packet, ip, port);
}

} // namespace net
