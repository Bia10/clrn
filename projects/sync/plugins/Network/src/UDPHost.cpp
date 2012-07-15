#include "stdafx.h"
#include "UDPHost.h"

namespace ba = boost::asio;

namespace net
{

//! Host implementation
class CUDPHost::Impl : boost::noncopyable
{
	//! Buffer type
	typedef std::vector<char>									Buffer;

	//! Buffer ptr type
	typedef boost::shared_ptr<Buffer>							BufferPtr;

	//! Buffer descriptor
	struct BufferDsc
	{
		BufferPtr					buffer;
		SocketPtr					socket;
		EndpointPtr					ep;
		boost::posix_time::ptime	time;
		packets::Packet_PacketType	type;
	};

	//! Waiting packets map type
	typedef std::map<std::string, BufferDsc> 					PacketsMap;

	//! Resend ratio
	enum														{RESEND_RATIO = 3};

	//! Delete ratio
	enum														{DELETE_RATIO = 6};

	//! Minimum waiting incoming threads
	enum														{INCOMING_THREADS_COUNT = 4};		

	//! Host map status refresh ratio
	//! refresh host_map table only if if pinginterval * HOST_MAP_REFRESH_RATIO expired
	enum														{HOST_MAP_REFRESH_RATIO = 3};

	//! Packet delete reason
	struct DeleteReason
	{
		enum Enum_t 
		{
			Delivered	= 0,
			TimedOut	= 1
		};
	};

public:
	Impl
	(
		IKernel& kernel, 
		ILog& log, 
		const std::string& remoteGuid,
		const SocketPtr srvSocket
	)
		: m_IOService(srvSocket->get_io_service())
		, m_Kernel(kernel)
		, m_Log(log)
		, m_BufferSize(kernel.Settings().BufferSize())
		, m_PingInterval(kernel.Settings().PingInterval())
		, m_RemoteGuid(remoteGuid)
		, m_LocalGuid(kernel.Settings().LocalGuid())
		, m_LocalHostName(ba::ip::host_name())
		, m_pSrvSocket(srvSocket)
		, m_OutgoingPing(std::numeric_limits<std::size_t>::max())
		, m_IncomingPing(std::numeric_limits<std::size_t>::max())
		, m_WaitingThreads(0)
		, m_OutgoigStatusLastUpdateTime(boost::posix_time::microsec_clock::local_time())
		, m_IncomingStatusLastUpdateTime(boost::posix_time::microsec_clock::local_time())
		, m_IncomingStatus(Status::Unknown)
		, m_LastOutgoingSend(boost::posix_time::microsec_clock::local_time())
	{
		 m_Kernel.Timer(boost::posix_time::milliseconds(m_PingInterval), boost::bind(&Impl::IncomingStatusTimerCallback, this));
		 m_Kernel.Timer(boost::posix_time::milliseconds(m_PingInterval), boost::bind(&Impl::PingHost, this));
	}

	void Send(const ProtoPacketPtr packet)
	{
		if (m_OutgoingPing < m_IncomingPing || !m_IncomingEP)
			Send2Outgoing(packet);
		else
			Send2Incoming(packet);
	}

	void PingHost()
	{
		m_Kernel.Timer(boost::posix_time::milliseconds(m_PingInterval), boost::bind(&Impl::PingHost, this));

		const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();

		{
			boost::mutex::scoped_lock lock(m_LastSendMutex);
			boost::posix_time::time_duration td(now - m_LastOutgoingSend);
			if (td.total_milliseconds() < m_PingInterval)
				return;

			m_LastOutgoingSend = now;
		}

		// sending ping packet to remote host
		const ProtoPacketPtr packet(new packets::Packet());
		packet->set_from(m_LocalGuid);
		packet->set_type(packets::Packet_PacketType_PING);
		packet->set_guid(CGUID::Generate());
		packet->set_timeout(static_cast<std::size_t>(conv::ToPosix64(boost::posix_time::microsec_clock::local_time())));
		packet->set_ep(m_LocalHostName + ":" + conv::cast<std::string>(m_pSrvSocket->local_endpoint().port()));

		Send2Outgoing(packet);
	}

	void SetOutgoingEP(const std::string& ip, const std::string& port, const std::size_t ping)
	{
		TRY 
		{
			m_OutgoingEP = MakeEndpoint(ip, port);
			m_OutgoingPing = ping;
		}
		CATCH_PASS_EXCEPTIONS(ip, port, ping)
	}

	void SetIncomingEP(const std::string& ip, const std::string& port, const std::size_t ping)
	{
		TRY 
		{
			m_IncomingEP = MakeEndpoint(ip, port);
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
				DeleteWaitingPacket(packet->guid(), DeleteReason::Delivered);
				return;
			}

			CHECK(!packet->from().empty(), "Host didn't send GUID, ignore packet.")
		
			// getting sender endpoint
			const std::string ep = client->address().to_string() + ":" + conv::cast<std::string>(client->port());
	
			// setting up packet field
			packet->set_ep(ep);

			// checking packet type
			if (packets::Packet_PacketType_PING == packet->type())
			{
				IncomingPingReceived(packet);
				m_IncomingEP = client;
			}
			else
			{
				// handle packet by kernel
				m_Kernel.HandleNewPacket(packet);
			}
		
			// sending ACK
			const ProtoPacketPtr ack(new packets::Packet());
			ack->set_from(m_LocalGuid);
			ack->set_guid(packet->guid());
			ack->set_type(packets::Packet_PacketType_ACK);
	
			const BufferPtr buffer(new Buffer());
			buffer->resize(ack->ByteSize());
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

	//! Make endpoint
	EndpointPtr MakeEndpoint(const std::string& ip, const std::string& port)
	{
		boost::asio::ip::udp::resolver resolver(m_IOService);

		boost::asio::ip::udp::resolver::query query(ip, port);
		boost::asio::ip::udp::resolver::iterator endpointIterator = resolver.resolve(query);
		boost::asio::ip::udp::resolver::iterator end;

		boost::system::error_code error = boost::asio::error::host_not_found;
		while(error && endpointIterator != end)
		{
			if (boost::asio::ip::udp::v4() == endpointIterator->endpoint().protocol())
			{
				return EndpointPtr(new Endpoint(*endpointIterator));
			}
			++endpointIterator;
		}

		CHECK(!ip.empty(), "Failed to find endpoint", ip, port, error.message());
		return EndpointPtr();
	}
	
	//! Send handler
	void SendHandler(const boost::system::error_code e, const std::size_t size, const BufferPtr /*sendBuffer*/)
	{
		if (e)
		{
			LOG_ERROR("Send error. Size: [%s], error: [%s]") % size % e.message();
		}
	}

	//! Send buffer
	void SendBuffer
	(
		const BufferPtr buffer, 
		const std::string& guid, 
		const EndpointPtr ep, 
		const SocketPtr socket, 
		const packets::Packet_PacketType type
	)
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

			// saving packet until ACK received or timeout expired
			{
				boost::mutex::scoped_lock lock(m_PacketsMutex);
				BufferDsc& dsc = m_WaitingPackets[guid];
				dsc.buffer = buffer;
				dsc.ep = ep;
				dsc.socket = socket;
				dsc.time = boost::posix_time::microsec_clock::local_time();
				dsc.type = type;
			}

			// setup timer for resend this data
			if (packets::Packet_PacketType_PING != type)
				m_Kernel.Timer(boost::posix_time::milliseconds(m_PingInterval * RESEND_RATIO), boost::bind(&Impl::ResendPacket, this, guid));

			// setting up timer to delete this packet
			m_Kernel.Timer(boost::posix_time::milliseconds(m_PingInterval * DELETE_RATIO), boost::bind(&Impl::DeleteWaitingPacket, this, guid, DeleteReason::TimedOut));
		}
		CATCH_PASS_EXCEPTIONS("SendBuffer failed.", guid, type)
	}

	//! Serialize buffer
	BufferPtr Serialize(const ProtoPacketPtr packet)
	{
		// setting up guid
		packet->set_from(m_LocalGuid);

		// sending
		const BufferPtr buffer(new Buffer());
		buffer->resize(packet->ByteSize());
		packet->SerializeToArray(&buffer->front(), buffer->size());

		return buffer;
	}

	//! Start receiving
	void StartReceiving(const SocketPtr socket, BufferPtr buffer = BufferPtr())
	{
		SCOPED_LOG(m_Log);

		TRY 
		{
			if (socket == m_pOutgoingSocket)
			{
				boost::mutex::scoped_lock lock(m_WaitingThreadsMutex);
				if (m_WaitingThreads >= INCOMING_THREADS_COUNT)
					return;
					
				++m_WaitingThreads;
			}

			if (!buffer)
				buffer.reset(new Buffer(m_BufferSize));

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
			const EndpointPtr ep(new Endpoint);
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

	//! Check and continue receiving
	void CheckAndContinueReceiving(const SocketPtr socket, const BufferPtr buffer)
	{
		if (socket == m_pSrvSocket)
			StartReceiving(socket, buffer);
		else 
		{
			{
				boost::mutex::scoped_lock lock(m_WaitingThreadsMutex);
				if (m_WaitingThreads)
					--m_WaitingThreads;
			}

			StartReceiving(socket, buffer);
		}
	}

	//! Receive data handle
	void ReceiveHandle
	(
		const boost::system::error_code e, 
		const std::size_t size, 
		const BufferPtr buffer, 
		const EndpointPtr client,
		const SocketPtr socket
	)
	{
		SCOPED_LOG(m_Log);

		TRY 
		{
			if (e || !size)
			{
				LOG_ERROR("Received size: [%s], error: [%s]") % size % e.message();
				CheckAndContinueReceiving(socket, buffer);
				return;
			}

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

			CheckAndContinueReceiving(socket, buffer);
		}
		CATCH_IGNORE_EXCEPTIONS(m_Log, "ReceiveHandle failed.")	
	}

	//! Send to outgoing channel
	void Send2Outgoing(const ProtoPacketPtr packet)
	{
		if (!m_OutgoingEP)
			return;

		{
			boost::mutex::scoped_lock lock(m_LastSendMutex);
			m_LastOutgoingSend = boost::posix_time::microsec_clock::local_time();
		}

		LOG_TRACE("Sending to: [%s], as [OUT], ep:[%s]:[%s], data: [%s]") 
			% m_RemoteGuid 
			% m_OutgoingEP->address().to_string() 
			% m_OutgoingEP->port() 
			% packet->ShortDebugString();

		// create socket if not exists and send data
		if (!m_pOutgoingSocket)
		{
			m_pOutgoingSocket.reset(new boost::asio::ip::udp::socket(m_IOService));
			m_pOutgoingSocket->open(boost::asio::ip::udp::v4());
		}

		SendBuffer(Serialize(packet), packet->guid(), m_OutgoingEP, m_pOutgoingSocket, packet->type());

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
		SendBuffer(Serialize(packet), packet->guid(), m_IncomingEP, m_pSrvSocket, packet->type());
	}

	//! Delete packet by guid
	void DeleteWaitingPacket(const std::string& packet, const DeleteReason::Enum_t reason)
	{
		boost::mutex::scoped_lock lock(m_PacketsMutex);

		if (!m_WaitingPackets.count(packet))
			return;

		const BufferDsc dsc = m_WaitingPackets[packet];

		const boost::posix_time::time_duration ping = boost::posix_time::microsec_clock::local_time() - dsc.time;

		LOG_TRACE("Deleting packet: [%s], to: [%s], reason: [%s], millisec: [%s]") 
			% packet 
			% m_LocalGuid 
			% (reason == DeleteReason::Delivered ? "delivered"  : "timeout")
			% static_cast<std::size_t>(ping.total_milliseconds());

		m_WaitingPackets.erase(packet);

		if (packets::Packet_PacketType_PING != dsc.type)
			return;

		m_OutgoingPing = static_cast<std::size_t>(ping.total_milliseconds());

		// checking up refresh timeout
		const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		const boost::posix_time::time_duration td(now - m_OutgoigStatusLastUpdateTime);
		if (td.total_milliseconds() < m_PingInterval * HOST_MAP_REFRESH_RATIO)
			return;

		m_OutgoigStatusLastUpdateTime = now;

		// checking for host timeout
		if (DeleteReason::TimedOut == reason)
		{
			// outgoing channel to host is timed out
			DeleteHostMapRecord(false);
		}
		else
		if (DeleteReason::Delivered == reason)
		{
			// outgoing channel to host established
			InsertHostMapRecord(false);
		}
	}

	//! Resend packet
	void ResendPacket(const std::string& packet)
	{
		TRY 
		{
			BufferDsc dsc;

			{
				boost::mutex::scoped_lock lock(m_PacketsMutex);

				if (!m_WaitingPackets.count(packet))
					return;

				dsc = m_WaitingPackets[packet];
			}

			LOG_TRACE("Resending packet: [%s], to: [%s]") % packet % m_LocalGuid;

			SendBuffer(dsc.buffer, packet, dsc.ep, dsc.socket, dsc.type);

			// setup timer for resend this data
			m_Kernel.Timer(boost::posix_time::milliseconds(m_PingInterval * RESEND_RATIO), boost::bind(&Impl::ResendPacket, this, packet));
		}
		CATCH_PASS_EXCEPTIONS(m_LocalGuid, packet)
	}

	//! Insert new host map record from packet
	void InsertHostMapRecord(const bool incoming)
	{
		TRY 
		{
			const EndpointPtr ep = incoming ? m_IncomingEP : m_OutgoingEP;

			CHECK(ep);

			const std::string ip = ep->address().to_string();
			const std::string port = conv::cast<std::string>(ep->port());

			const std::string ping = conv::cast<std::string>(incoming ? m_IncomingPing : m_OutgoingPing);

			// insert host map
			CProcedure script(m_Kernel);
			CProcedure::ParamsMap mapParams;

			mapParams["from"]		= incoming ? m_RemoteGuid : m_LocalGuid;
			mapParams["to"]			= incoming ? m_LocalGuid : m_RemoteGuid;
			mapParams["status"]		= conv::cast<std::string>(Status::SessionEstablished);
			mapParams["ping"]		= ping;
			mapParams["ip"]			= ip;
			mapParams["port"]		= port;	

			script.Execute(CProcedure::Id::HostMapCreate, mapParams, IJob::CallBackFn());	
		}
		CATCH_PASS_EXCEPTIONS(incoming)
	}

	//! Delete host map record
	void DeleteHostMapRecord(const bool incoming)
	{
		TRY 
		{
			// erasing host map data
			CProcedure proc(m_Kernel);
			CProcedure::ParamsMap params;
			params["from"]	= incoming ? m_RemoteGuid : m_LocalGuid;
			params["to"]	= incoming ? m_LocalGuid : m_RemoteGuid;
			proc.Execute(CProcedure::Id::HostMapDelete, params, IJob::CallBackFn());	
		}
		CATCH_PASS_EXCEPTIONS(incoming)
	}

	//! Incoming ping received
	void IncomingPingReceived(const ProtoPacketPtr packet)
	{
		boost::mutex::scoped_lock lock(m_PacketsMutex);

		const boost::posix_time::ptime time = conv::FromPosix64(packet->timeout());

		const boost::posix_time::time_duration td(boost::posix_time::microsec_clock::local_time() - time);
		m_IncomingPing = static_cast<std::size_t>(td.total_milliseconds());

		m_IncomingStatus = Status::SessionEstablished;
	}

	//! Incoming status control callback
	void IncomingStatusTimerCallback()
	{
		m_Kernel.Timer(boost::posix_time::milliseconds(m_PingInterval), boost::bind(&Impl::IncomingStatusTimerCallback, this));

		boost::mutex::scoped_lock lock(m_PacketsMutex);

		// checking up refresh timeout
		const boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
		const boost::posix_time::time_duration td(now - m_IncomingStatusLastUpdateTime);
		if (td.total_milliseconds() < m_PingInterval * HOST_MAP_REFRESH_RATIO)
			return;

		m_IncomingStatusLastUpdateTime = now;

		// update status
		if (Status::SessionEstablished == m_IncomingStatus)
			InsertHostMapRecord(true);
		else
			DeleteHostMapRecord(true);
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

	//! Local host name
	const std::string					m_LocalHostName;

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

	//! Waiting for incoming connection threads count 
	std::size_t							m_WaitingThreads;

	//! Waiting threads mutex
	boost::mutex						m_WaitingThreadsMutex;

	//! Outgoing status last update time
	boost::posix_time::ptime			m_OutgoigStatusLastUpdateTime;

	//! Outgoing status last update time
	boost::posix_time::ptime			m_IncomingStatusLastUpdateTime;

	//! Incoming ping status
	Status::Enum_t						m_IncomingStatus;

	//! Last send to outgoing channel
	boost::posix_time::ptime			m_LastOutgoingSend;

	//! Last outgoing send mutex
	boost::mutex						m_LastSendMutex;
};

CUDPHost::CUDPHost
(
	IKernel& kernel, 
	ILog& log, 
	const std::string& remoteGuid,
	const SocketPtr srvSocket
)
	: m_pImpl(new Impl(kernel, log, remoteGuid, srvSocket))
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

} // namespace net
