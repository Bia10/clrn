#ifndef UDPHost_h__
#define UDPHost_h__

#include "ProtoPacketPtr.h"

#include <memory>

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

namespace net
{

//! Encapsulates udp remote host
class CUDPHost : boost::noncopyable
{
public:

	//! Host ptr type
	typedef boost::shared_ptr<CUDPHost>		Ptr;

	//! Socket type
	typedef boost::asio::ip::udp::socket	Socket;

	//! Socket ptr type
	typedef boost::shared_ptr<Socket>		SocketPtr;

	//! Endpoint type
	typedef boost::asio::ip::udp::endpoint	Endpoint;

	//! Endpoint ptr
	typedef  boost::shared_ptr<Endpoint>	EndpointPtr;

	CUDPHost
	(
		boost::asio::io_service& srvc, 
		IKernel& kernel, 
		ILog& log, 
		const std::size_t bufferSize,
		const std::size_t pingInterval,
		const std::string& remoteGuid,
		const std::string& localGuid,
		const SocketPtr srvSocket
	);

	~CUDPHost(void);

	//! Send data packet to host
	void	Send(const ProtoPacketPtr packet);

	//! Send ping data packet
	void	SendPingPacket(const ProtoPacketPtr packet, const std::string& ip, const std::string& port);

	//! Set host outgoing endpoint
	void	SetOutgoingEP(const std::string& ip, const std::string& port, const std::size_t ping);

	//! Set host incoming endpoint
	void	SetIncomingEP(const std::string& ip, const std::string& port, const std::size_t ping);

	//! Handle incoming packet
	void	HandlePacket(const SocketPtr socket, const ProtoPacketPtr packet, const EndpointPtr client);

private:

	//! Implementation
	class Impl;
	std::auto_ptr<Impl> m_pImpl;
};

} // namespace net

#endif // UDPHost_h__
