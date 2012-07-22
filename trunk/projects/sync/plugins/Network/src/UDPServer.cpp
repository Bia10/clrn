#include "stdafx.h"
#include "UDPServer.h"
#include "UDPHost.h"

namespace net
{

//! UDP server implementation
//! 
//! \class CUDPServer::Impl
//!
class CUDPServer::Impl : boost::noncopyable
{
	//! Service type
	typedef boost::asio::io_service								Service;

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
		Service& srvc
	)
		: m_Log(logger)
		, m_Kernel(kernel)
		, m_Service(srvc)
		, m_BufferSize(kernel.Settings().BufferSize())
		, m_LocalGUID(kernel.Settings().LocalGuid())
		, m_PingInterval(kernel.Settings().PingInterval())
	{
		SCOPED_LOG(m_Log);
		Init(m_Kernel.Settings().UDPPort(), m_Kernel.Settings().ThreadsCount());
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

	void Init(const int port, const std::size_t threads)
	{
		SCOPED_LOG(m_Log);
		
		LOG_TRACE("Port: [%s], buffer: [%s]") % port % m_BufferSize;

		TRY 
		{
			// receive socket
			m_pReceiveSocket.reset(new boost::asio::ip::udp::socket(m_Service, Endpoint(boost::asio::ip::udp::v4(), conv::cast<unsigned short>(port))));
	
			for (std::size_t i = 0; i < threads; ++i)
				StartReceiving(m_pReceiveSocket);
		}
		CATCH_PASS_EXCEPTIONS("Init failed.", port)
	}

	//! Start receiving
	void StartReceiving(const SocketPtr socket)
	{
		SCOPED_LOG(m_Log);

		TRY 
		{
			const BufferPtr buffer(new std::vector<char>(m_BufferSize));	
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
				ContinueReceiving(buffer, socket);
				return;
			}

			const ProtoPacketPtr packet(new packets::Packet());
			if (packet->ParseFromArray(&buffer->front(), size))
			{
				LOG_TRACE("Received from: [%s], socket: [IN], ep: [%s]:[%s]") 
					% packet->from()
					% client->address().to_string() 
					% client->port();

				TRACE_PACKET(packet);
				HandlePacket(socket, packet, client);
			}

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
				{
					// checking for this host existence, create if needed
					const std::string ep = packet->ep().empty() ? client->address().to_string(), conv::cast<std::string>(client->port()) : packet->ep();

					CreateNewHost(packet->from(), ep);
					it = m_Hosts.insert(std::make_pair(packet->from(), CreateHost(packet->from()))).first;
				}
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

	//! Add host mapping from host map event
	void AddHostMapping(const ProtoPacketPtr packet)
	{
		TRACE_PACKET(packet);

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
		TRACE_PACKET(packet);

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
				it->second->SetIncomingEP(row["ip"], row["port"], std::numeric_limits<std::size_t>::max());
			else
				it->second->SetOutgoingEP(row["ip"], row["port"], std::numeric_limits<std::size_t>::max());
		}	
	}

	//! Add host mapping from hosts list
	void AddHosts(const ProtoPacketPtr packet)
	{
		TRACE_PACKET(packet);

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

			it->second->SetOutgoingEP(row["ip"], row["port"], std::numeric_limits<std::size_t>::max());
		}	
	}

	//! Create host
	CUDPHost::Ptr CreateHost(const std::string& guid)
	{
		return CUDPHost::Ptr(new CUDPHost
		(
			m_Kernel,
			m_Log,
			guid,
			m_pReceiveSocket
		));
	}

	//! Set ping interval
	void SetPingInterval(const std::size_t interval)
	{
		boost::mutex::scoped_lock lock(m_SettingsMutex);
		m_PingInterval = interval;
	}

	//! Check for new host
	void CreateNewHost(const std::string& guid, const std::string& ep)
	{
		const std::string::size_type delimiter = ep.find(':');

		CHECK(std::string::npos != delimiter);

		// insert host
		CProcedure script(m_Kernel);
		CProcedure::ParamsMap mapParams;

		mapParams["guid"]		= guid;
		mapParams["ip"]			= ep.substr(0, delimiter);
		mapParams["port"]		= &ep[delimiter + 1];	

		script.Execute(CProcedure::Id::HostsCreate, mapParams, IJob::CallBackFn());	
	}


private:

	//! Logger
	ILog&				m_Log;

	//! Kernel
	IKernel&			m_Kernel;

	//! Service
	Service&			m_Service;

	//! Buffer size
	std::size_t			m_BufferSize;

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

CUDPServer::CUDPServer(ILog& logger, IKernel& kernel, boost::asio::io_service& srvc)
	: m_pImpl(new Impl(logger, kernel, srvc))
{
}

CUDPServer::~CUDPServer(void)
{
}

void CUDPServer::SetBufferSize(const int size)
{
	m_pImpl->SetBufferSize(size);
}

void CUDPServer::Send(const std::string& destination, const ProtoPacketPtr packet)
{
	m_pImpl->Send(destination, packet);
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

