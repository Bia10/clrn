#ifndef UDPServer_h__
#define UDPServer_h__

#include "ILog.h"
#include "ProtoPacketPtr.h"

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

//! Forward declarations
class IKernel;

namespace boost
{
	namespace asio
	{
		class io_service;
	}
}

namespace net
{

//! UDP server interface
//! 
//! \class CUDPServer
//!
class CUDPServer : private boost::noncopyable
{
public:
	CUDPServer(ILog& logger, 
				IKernel& kernel,
				boost::asio::io_service& srvc,
				const int threads,
				const int port, 
				const int bufferSize,
				const std::string& guid);

	~CUDPServer(void);

	//! Set buffer size
	void				SetBufferSize(const int size);

	//! Send
	void				Send(const std::string& destination, const ProtoPacketPtr packet);
	
	//! Send to ep
	void				Send(const std::string& destination, const std::string& ip, const std::string& port, const ProtoPacketPtr packet);

	//! Add hosts
	void				AddHosts(const ProtoPacketPtr packet);

	//! Add host mapping
	void				AddHostMapping(const ProtoPacketPtr packet);

	//! Remove host mapping
	void				RemoveHostMapping(const ProtoPacketPtr packet);

	//! Set ping interval
	void				SetPingInterval(const std::size_t interval);

private:

	//! Implementation
	class Impl;
	boost::scoped_ptr<Impl> m_pImpl;
};

} // namespace net
#endif // UDPServer_h__