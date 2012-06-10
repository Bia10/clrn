#include "stdafx.h"
#include "UDPServer.h"
#include "UDPHost.h"

namespace net
{

//! UDP server implementation
//! 
//! \class CUDPServer
//!
class CUDPServer::Impl
{
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

	//! Hosts
	typedef std::map<std::string, CUDPHost::Ptr>				HostsMap;

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

			const ProtoPacketPtr packet(new packets::Packet());
			if (packet->ParseFromArray(&buffer->front(), size))
			{
				LOG_TRACE("Received from: [%s], socket: [IN], ep: [%s]:[%s]") 
					% packet->from()
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
	void HandlePacket(const SocketPtr socket, const ProtoPacketPtr packet, const EndpointPtr client)
	{
		SCOPED_LOG(m_Log);

		CHECK(packet);

		TRY 
		{
			// handle by host
			HostsMap::iterator it;
			{
				boost::mutex::scoped_lock lock(m_HostsMutex);
				it = m_Hosts.find(packet->from());
				if (m_Hosts.end() == it)
					it = m_Hosts.insert(std::make_pair(packet->from(), CreateHost(packet->from()))).first;
			}

			it->second->HandlePacket(socket, packet, client);		
		}
		CATCH_PASS_EXCEPTIONS(*packet)
	}

	//! Send packet
	void Send(const std::string destination, const ProtoPacketPtr packet)
	{
		SCOPED_LOG(m_Log);

		CHECK(packet);

		TRY 
		{
			// send by host
			HostsMap::iterator it;
			{
				boost::mutex::scoped_lock lock(m_HostsMutex);
				it = m_Hosts.find(destination);
				if (m_Hosts.end() == it)
					it = m_Hosts.insert(std::make_pair(destination, CreateHost(destination))).first;
			}

			it->second->Send(packet);		
		}
		CATCH_PASS_EXCEPTIONS("Send failed.", destination, *packet)
	}

	//! Send packet to endpoint
	void Send(const std::string& destination, const std::string& ip, const std::string& port, const ProtoPacketPtr packet)
	{
		SCOPED_LOG(m_Log);

		CHECK(packet);

		TRY 
		{
			// send by host
			HostsMap::iterator it;
			{
				boost::mutex::scoped_lock lock(m_HostsMutex);
				it = m_Hosts.find(destination);
			}
			CHECK(m_Hosts.end() != it);

			it->second->SendPingPacket(packet, ip, port);		
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
		CTable table(*packet->mutable_job()->mutable_results(0));

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

			HostsMap::iterator it;
			{
				boost::mutex::scoped_lock lock(m_HostsMutex);
				it = m_Hosts.find(key);
				if (m_Hosts.end() == it)
					it = m_Hosts.insert(std::make_pair(key, CreateHost(key))).first;
			}

			if (incoming)
				it->second->SetIncomingEP(row["ip"], row["port"], conv::cast<std::size_t>(row["ping"]));
			else
				it->second->SetOutgoingEP(row["ip"], row["port"], conv::cast<std::size_t>(row["ping"]));
		}	
	}

	//! Erase host mapping from host map event
	void RemoveHostMapping(const ProtoPacketPtr packet)
	{
		CTable table(*packet->mutable_job()->mutable_results(0));

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

			HostsMap::iterator it;
			{
				boost::mutex::scoped_lock lock(m_HostsMutex);
				it = m_Hosts.find(key);
				if (m_Hosts.end() == it)
					it = m_Hosts.insert(std::make_pair(key, CreateHost(key))).first;
			}

			if (incoming)
				it->second->SetIncomingEP(row["ip"], row["port"], std::size_t(-1));
			else
				it->second->SetOutgoingEP(row["ip"], row["port"], std::size_t(-1));
		}	
	}

	//! Add host mapping from hosts list
	void AddHosts(const ProtoPacketPtr packet)
	{
		CTable table(*packet->mutable_job()->mutable_results(0));

		BOOST_FOREACH(const CTable::Row& row, table)
		{
			const std::string& key = row["guid"];
			if (key == m_LocalGUID)
				continue;

			HostsMap::iterator it;
			{
				boost::mutex::scoped_lock lock(m_HostsMutex);
				it = m_Hosts.find(key);
				if (m_Hosts.end() == it)
					it = m_Hosts.insert(std::make_pair(key, CreateHost(key))).first;
			}

			it->second->SetOutgoingEP(row["ip"], row["port"], std::size_t(-1));
		}	
	}

	//! Create host
	CUDPHost::Ptr CreateHost(const std::string& guid)
	{
		return CUDPHost::Ptr(new CUDPHost
		(
			*m_pIOService,
			m_Kernel,
			m_Log,
			m_BufferSize,
			m_PingInterval,
			guid,
			m_LocalGUID,
			m_pReceiveSocket
		));
	}

	//! Set ping interval
	void SetPingInterval(const std::size_t interval)
	{
		boost::mutex::scoped_lock lock(m_SettingsMutex);
		m_PingInterval = interval;
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

	//! Local host guid
	std::string			m_LocalGUID;

	//! Mutex for server settings
	boost::mutex		m_SettingsMutex;

	//! Ping interval
	std::size_t			m_PingInterval;

	//! Hosts mutex
	boost::mutex		m_HostsMutex;

	//! Hosts 
	HostsMap			m_Hosts;

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

void CUDPServer::Send(const std::string& destination, const std::string& ip, const std::string& port, const ProtoPacketPtr packet)
{
	m_pImpl->Send(destination, ip, port, packet);
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

