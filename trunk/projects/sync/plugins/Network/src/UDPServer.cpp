#include "stdafx.h"
#include "UDPServer.h"

namespace net
{

//! UDP server implementation
//! 
//! \class CUDPServer
//!
class CUDPServer::Impl
{
	//! Host info
	struct HostInfo
	{
		std::string ip;
		std::string port;

		bool operator < (const HostInfo& that) const
		{
			if (ip == that.ip)
				return port < that.port;

			return ip < that.ip;
		}
	};

	//! Connection info
	struct Connection
	{
		bool		incoming;
		HostInfo	ep;
		std::size_t ping;
	};

	//! Type of the pointer to the boost::asio service
	typedef boost::shared_ptr<boost::asio::io_service> 			ServicePtr;

	//! Type of the work IO service pointer
	typedef boost::shared_ptr<boost::asio::io_service::work> 	WorkPtr;

	//! Type of the boost::asio signal set pointer
	typedef boost::scoped_ptr<boost::asio::signal_set> 			SignalSetPtr;

	//! Server receive endpoint
	typedef boost::asio::ip::udp::endpoint						Endpoint;

	//! Endpoint ptr
	typedef  boost::shared_ptr<Endpoint>						EndpointPtr;

	//! Buffer type
	typedef boost::shared_ptr< std::vector<char> >				BufferPtr;

	//! Socket type
	typedef boost::shared_ptr<boost::asio::ip::udp::socket>		SocketPtr;

	//! Packet ptr type
	typedef boost::shared_ptr<packets::Packet>					PacketPtr;

	//! Resolved endpoints map
	typedef std::map<HostInfo, EndpointPtr>						HostMap;

	//! Connections map
	typedef std::multimap<std::string, Connection>				ConnectionsMap;

	//! Connections iterator range
	typedef std::pair<ConnectionsMap::const_iterator, ConnectionsMap::const_iterator> ConnectionsRange;

	//! Waiting packets map type
	typedef std::map<std::string, std::map<std::string, BufferPtr> >	PacketsMap;

public:

	Impl
	(
		ILog& logger, 
		IKernel& kernel,
		const int port, 
		const int threads,
		const int bufferSize,
		const std::string& guid
	)
		: m_Log(logger)
		, m_Kernel(kernel)
		, m_BufferSize(bufferSize)
		, m_LocalGUID(guid)
		, m_IsReceiving(false)
		, m_PingInterval(0)
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

		CHECK(m_BufferSize, size);
		m_BufferSize = size;
	}

	void Init(const int port, const int threads)
	{
		SCOPED_LOG(m_Log);
		
		LOG_TRACE("Port: [%s], threads: [%s], buffer: [%s]") % port % threads % m_BufferSize;

		TRY 
		{
			m_pIOService.reset(new boost::asio::io_service(threads));
			m_pWork.reset(new boost::asio::io_service::work(*m_pIOService));
	
			for (int i = 0; i < threads; ++i)
			{
				m_ThreadPool.create_thread(boost::bind(&boost::asio::io_service::run, m_pIOService));
			}
	
			// Creating signal set
			m_pSignals.reset(new boost::asio::signal_set(*m_pIOService));
	
			boost::asio::ip::udp::resolver resolver(*m_pIOService);
		
			// receive socket
			m_pReceiveSocket.reset(new boost::asio::ip::udp::socket(*m_pIOService, Endpoint(boost::asio::ip::udp::v4(), conv::cast<unsigned short>(port))));

			// send socket
			m_pSendSocket.reset(new boost::asio::ip::udp::socket(*m_pIOService));
			m_pSendSocket->open(boost::asio::ip::udp::v4());
	
			// Register to handle the signals that indicate when the server should exit.
			// It is safe to register for the same signal multiple times in a program,
			// provided all registration for the specified signal is made through Asio.
			m_pSignals->add(SIGINT);
			m_pSignals->add(SIGTERM);
	
	#if defined(SIGQUIT)
			m_pTerminationSignals->add(SIGQUIT);
	#endif // defined(SIGQUIT)
	
			m_pSignals->async_wait(boost::bind(&Impl::Stop, this));

			for (std::size_t i = 0; i < m_ThreadPool.size(); ++i)
				StartReceiving(m_pReceiveSocket);
		}
		CATCH_PASS_EXCEPTIONS("Init failed.", port, threads)
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

			const PacketPtr packet(new packets::Packet());
			if (packet->ParseFromArray(&buffer->front(), size))
			{
				LOG_TRACE("Received from: [%s], socket: [%s], ep: [%s]:[%s]") 
					% packet->from()
					% (socket == m_pReceiveSocket ? "receive" : "send")
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

	//! Handle new packet
	void HandlePacket(const SocketPtr socket, const PacketPtr packet, const EndpointPtr client)
	{
		SCOPED_LOG(m_Log);

		CHECK(packet);

		TRY 
		{
			// checking packet type
			if (packets::Packet_PacketType_ACK == packet->type())
			{
				DeleteWaitingPacket(packet->from(), packet->guid(), "delivered");
				return;
			}
	
			HostInfo host;
			host.ip = client->address().to_string();
			host.port = conv::cast<std::string>(client->port());
			const HostMap::const_iterator it = m_Hosts.find(host);
			if (m_Hosts.end() == it)
			{
				boost::mutex::scoped_lock lock(m_HostMutex);
				m_Hosts.insert(std::make_pair(host, EndpointPtr(new Endpoint(*client))));
			}
	
			// getting sender endpoint
			const std::string ep = host.ip + ":" + host.port;
	
			// setting up packet field
			packet->set_ep(ep);
	
			// handle packet by kernel
			m_Kernel.HandleNewPacket(packet);
	
			// sending ACK
			const PacketPtr ack(new packets::Packet());
			ack->set_from(m_LocalGUID);
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
					client,
					buffer
				)
			);	
		}
		CATCH_PASS_EXCEPTIONS(*packet)
	}

	//! Get endpoint
	EndpointPtr GetEndpoint(const std::string& dest, bool& incomingChannel)
	{
		SCOPED_LOG(m_Log);

		boost::mutex::scoped_lock lock(m_ConnectionsMutex);

		CHECK(m_Connections.count(dest));

		// find connection by guid, sort by ping
		const ConnectionsRange range = m_Connections.equal_range(dest);

		CHECK(m_Connections.end() != range.first && range.first != range.second);

		std::vector<Connection> connections;
		for (ConnectionsMap::const_iterator it = range.first; it != range.second; ++it)
		{
			connections.push_back(it->second);
		}

		CHECK(!connections.empty());

		// sort connections by ping
		struct SortByPing
		{
			bool operator () (const Connection& lhs, const Connection& rhs) const
			{
				return rhs.ping < lhs.ping;
			}
		};

		std::sort(connections.begin(), connections.end(), SortByPing());

		// getting ip and port
		const HostInfo& host = connections.front().ep;
		incomingChannel = connections.front().incoming;

		// finding endpoint by info
		HostMap::const_iterator it;
		{
			boost::mutex::scoped_lock lock(m_HostMutex);
			it = m_Hosts.find(host);	
		}
		if (m_Hosts.end() != it)
			return it->second;
	
		boost::asio::ip::udp::resolver resolver(*m_pIOService);
		const boost::asio::ip::udp::resolver::query addressQuery(host.ip, host.port);

		const boost::asio::ip::udp::resolver::iterator ep = resolver.resolve(addressQuery);

		{
			boost::mutex::scoped_lock lock(m_HostMutex);
			it = (m_Hosts.insert(std::make_pair(host, EndpointPtr(new Endpoint(*ep))))).first;
		}

		return it->second;
	}

	//! Send buffer
	void SendBuffer(const BufferPtr buffer, const EndpointPtr ep, bool receiveSocket)
	{
		SCOPED_LOG(m_Log);

		CHECK(buffer);

		TRY 
		{
			const SocketPtr socket = receiveSocket ? m_pReceiveSocket : m_pSendSocket;
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
					ep,
					buffer
				)
			);	

			if (!m_IsReceiving && !receiveSocket)
			{
				m_IsReceiving = true;
				StartReceiving(m_pSendSocket);
			}
		}
		CATCH_PASS_EXCEPTIONS("SendBuffer failed.")
	}

	//! Send packet
	void Send(const std::string destination, const ProtoPacketPtr packet)
	{
		SCOPED_LOG(m_Log);

		CHECK(packet);

		TRY 
		{
			bool receiveSocket = true;
			const EndpointPtr ep = GetEndpoint(destination, receiveSocket);

			// setting up guid
			packet->set_from(m_LocalGUID);

			// sending
			const BufferPtr buffer(new std::vector<char>(packet->ByteSize()));
			packet->SerializeToArray(&buffer->front(), buffer->size());

			// actually send
			SendBuffer(buffer, ep, receiveSocket);

			// setup timers for delete/resend this data
			RegisterPacket(destination, packet->guid(), buffer);

			if (!m_IsReceiving && !receiveSocket)
			{
				m_IsReceiving = true;
				StartReceiving(m_pSendSocket);
			}

			LOG_TRACE("Sended to: [%s], socket: [%s], ep: [%s]:[%s]") 
				% destination
				% (receiveSocket ? "receive" : "send")
				% ep->address().to_string() 
				% ep->port();
		}
		CATCH_PASS_EXCEPTIONS("Send failed.", destination, *packet)
	}

	//! Send packet to endpoint
	void Send(const std::string& ip, const std::string& port, const ProtoPacketPtr packet)
	{
		SCOPED_LOG(m_Log);

		CHECK(packet);

		TRY 
		{
			HostInfo host;
			host.ip = ip;
			host.port = port;
		
			HostMap::const_iterator it = m_Hosts.find(host);
			if (m_Hosts.end() == it)
			{
				boost::asio::ip::udp::resolver resolver(*m_pIOService);
				const boost::asio::ip::udp::resolver::query addressQuery(host.ip, host.port);
				const boost::asio::ip::udp::resolver::iterator ep = resolver.resolve(addressQuery);

				boost::mutex::scoped_lock lock(m_HostMutex);
				it = (m_Hosts.insert(std::make_pair(host, EndpointPtr(new Endpoint(*ep))))).first;
			}

			const EndpointPtr ep = it->second;

			// setting up guid
			packet->set_from(m_LocalGUID);

			// sending
			const BufferPtr buffer(new std::vector<char>(packet->ByteSize()));
			packet->SerializeToArray(&buffer->front(), buffer->size());

			// actual send
			SendBuffer(buffer, ep, false);

			if (!m_IsReceiving)
			{
				m_IsReceiving = true;
				StartReceiving(m_pSendSocket);
			}

			LOG_TRACE("Sended to: [%s]:[%s] socket: [%s], ep: [%s]:[%s]") 
				% ip
				% port
				% "send"
				% ep->address().to_string() 
				% ep->port();
		}
		CATCH_PASS_EXCEPTIONS("Send failed.", ip, port, *packet)	
	}


	//! Send handler
	void SendHandler(const boost::system::error_code e, const std::size_t size, const EndpointPtr ep, const BufferPtr /*sendBuffer*/)
	{
		if (e)
		{
			LOG_ERROR("Send error. Size: [%s], error: [%s]") % size % e.message();
			return;
		}
	}

	//! Add host mapping from host map event
	void AddHostMapping(const ProtoPacketPtr packet)
	{
		boost::mutex::scoped_lock lock(m_ConnectionsMutex);

		CTable table(*packet->mutable_job()->mutable_results(0));

		const std::string debug = packet->DebugString();

		BOOST_FOREACH(const CTable::Row& row, table)
		{
			Connection connection;

			const std::string& from = row["from"];
			const std::string& to = row["to"];

			std::string key;
			if (from == m_LocalGUID)
			{
				connection.incoming = false;
				key = to;
			}
			else
			{
				connection.incoming = true;
				key = from;
			}

			connection.ep.ip = row["ip"];
			connection.ep.port = row["port"];

			connection.ping = conv::cast<std::size_t>(row["ping"]);

			bool updated = false;
			const std::pair<ConnectionsMap::iterator, ConnectionsMap::iterator> range = m_Connections.equal_range(key);
			for (ConnectionsMap::iterator it = range.first; it != range.second; ++it)
			{
				if (it->second.incoming == connection.incoming)
				{
					updated = true;
					it->second = connection;
				}
			}		

			if (!updated)
				m_Connections.insert(std::make_pair(key, connection));
		}	
	}

	//! Erase host mapping from host map event
	void RemoveHostMapping(const ProtoPacketPtr packet)
	{
		boost::mutex::scoped_lock lock(m_ConnectionsMutex);

		CTable table(*packet->mutable_job()->mutable_results(0));

		std::vector<ConnectionsMap::const_iterator> toErase;
		BOOST_FOREACH(const CTable::Row& row, table)
		{
			const std::string& from = row["from"];
			const std::string& to = row["to"];

			std::string key;
			bool incoming = false;
			if (from == m_LocalGUID)
			{
				incoming = false;
				key = to;
			}
			else
			{
				incoming = true; 
				key = from;
			}

			const ConnectionsRange range = m_Connections.equal_range(key);

			for (ConnectionsMap::const_iterator it = range.first; it != range.second; ++it)
			{
				if (it->second.incoming == incoming)
					toErase.push_back(it);
			}
		}	

		// erase data
		BOOST_FOREACH(const ConnectionsMap::const_iterator& it, toErase)
		{
			m_Connections.erase(it);
		}
	}

	//! Add host mapping from hosts list
	void AddHosts(const ProtoPacketPtr packet)
	{
		boost::mutex::scoped_lock lock(m_ConnectionsMutex);

		CTable table(*packet->mutable_job()->mutable_results(0));

		BOOST_FOREACH(const CTable::Row& row, table)
		{
			Connection connection;

			const std::string& key = row["guid"];
			if (key == m_LocalGUID)
				continue;

			connection.incoming		= false;
			connection.ep.ip		= row["ip"];
			connection.ep.port		= row["port"];
			connection.ping			= std::size_t(-1);

			m_Connections.insert(std::make_pair(key, connection));
		}	
	}

	//! Set ping interval
	void SetPingInterval(const std::size_t interval)
	{
		boost::mutex::scoped_lock lock(m_SettingsMutex);
		m_PingInterval = interval;
	}

	//! Delete packet by guids
	void DeleteWaitingPacket(const std::string& host, const std::string& packet, const std::string& reason)
	{
		boost::mutex::scoped_lock lock(m_PacketsMutex);

		if (!m_OutgoingPackets.count(host))
			return;

		if (!m_OutgoingPackets[host].count(packet))
			return;

		LOG_TRACE("Deleting packet: [%s], to: [%s], reason: [%s]") % packet % host % reason;
		m_OutgoingPackets[host].erase(packet);
	}

	//! Resend packet
	void ResendPacket(const std::string& host, const std::string& packet)
	{
		TRY 
		{
			boost::mutex::scoped_lock lock(m_PacketsMutex);
	
			if (!m_OutgoingPackets.count(host))
				return;
	
			if (!m_OutgoingPackets[host].count(packet))
				return;

			LOG_TRACE("Resending packet: [%s], to: [%s]") % packet % host;
	
			bool incoming = false;
			const EndpointPtr ep = GetEndpoint(host, incoming);
	
			SendBuffer(m_OutgoingPackets[host][packet], ep, incoming);

			boost::asio::deadline_timer timer(*m_pIOService, boost::posix_time::milliseconds(m_PingInterval));
			timer.async_wait(boost::bind(&Impl::ResendPacket, this, host, packet));
		}
		CATCH_PASS_EXCEPTIONS(host)
	}

	//! Register outgoing packet
	void RegisterPacket(const std::string& host, const std::string& packet, const BufferPtr buffer)
	{
		boost::mutex::scoped_lock lock(m_PacketsMutex);
		m_OutgoingPackets[host][packet] = buffer;

		// setup timer for resend this data
		boost::asio::deadline_timer resendTimer(*m_pIOService, boost::posix_time::milliseconds(m_PingInterval));
		resendTimer.async_wait(boost::bind(&Impl::ResendPacket, this, host, packet));

		// setting up timer to delete this packet
		boost::asio::deadline_timer deleteTimer(*m_pIOService, boost::posix_time::milliseconds(m_PingInterval * 6));
		deleteTimer.async_wait(boost::bind(&Impl::DeleteWaitingPacket, this, host, packet, "timed out"));
	}
	
	//! Run server
	void Run()
	{
		m_ThreadPool.join_all();
	}

	//! Stop server
	void Stop()
	{
		SCOPED_LOG(m_Log);
			
		m_pIOService->stop();
		m_ThreadPool.interrupt_all();
		m_ThreadPool.join_all();
	}

private:

	//! Logger
	ILog&				m_Log;

	//! Kernel
	IKernel&			m_Kernel;

	//! Buffer size
	std::size_t			m_BufferSize;

	//! IO service
	ServicePtr			m_pIOService;

	//! IO service dummy work
	WorkPtr				m_pWork;

	//! Thread pool
	boost::thread_group	m_ThreadPool;

	//! Signals
	SignalSetPtr		m_pSignals;

	//! Server receive socket
	SocketPtr			m_pReceiveSocket;

	//! Server send socket
	SocketPtr			m_pSendSocket;

	//! Resolved hosts
	HostMap				m_Hosts;

	//! Hosts mutex
	boost::mutex		m_HostMutex;

	//! Local host guid
	std::string			m_LocalGUID;

	//! Is receiving
	bool				m_IsReceiving;

	//! Mapping table mutes
	boost::mutex		m_ConnectionsMutex;

	//! Connections
	ConnectionsMap		m_Connections;

	//! Mutex for server settings
	boost::mutex		m_SettingsMutex;

	//! Ping interval
	std::size_t			m_PingInterval;

	//! Outgoing packets
	PacketsMap			m_OutgoingPackets;
	
	//! Outgoing packets mutex
	boost::mutex		m_PacketsMutex;
};

CUDPServer::CUDPServer(ILog& logger, IKernel& kernel, const int port, const int threads, const int bufferSize, const std::string& guid)
	: m_pImpl(new Impl(logger, kernel, port, threads, bufferSize, guid))
{
}

CUDPServer::~CUDPServer(void)
{
}

void CUDPServer::SetBufferSize(const int size)
{
	m_pImpl->SetBufferSize(size);
}

void CUDPServer::Run()
{
	m_pImpl->Run();
}

void net::CUDPServer::Stop()
{
	m_pImpl->Stop();
}

void CUDPServer::Send(const std::string& destination, const ProtoPacketPtr packet)
{
	m_pImpl->Send(destination, packet);
}

void CUDPServer::Send(const std::string& ip, const std::string& port, const ProtoPacketPtr packet)
{
	m_pImpl->Send(ip, port, packet);
}

void CUDPServer::AddHostMapping(const ProtoPacketPtr packet)
{
	m_pImpl->AddHostMapping(packet);
}

void CUDPServer::RemoveHostMapping(const ProtoPacketPtr packet)
{
	m_pImpl->RemoveHostMapping(packet);
}

void CUDPServer::AddHosts(const ProtoPacketPtr packet)
{
	m_pImpl->AddHosts(packet);
}

void CUDPServer::SetPingInterval(const std::size_t interval)
{
	m_pImpl->SetPingInterval(interval);
}

} // namespace net

