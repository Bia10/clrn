#ifndef Host_h__
#define Host_h__

#include "ProtoPacketPtr.h"

#include <memory>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

// Forward declarations
class IKernel;
class ILog;

//! Host implementation
class CPingHost : boost::noncopyable
{
public:

	//! Pointer type
	typedef boost::shared_ptr<CPingHost> Ptr;

	//! Host status
	struct Status
	{
		enum Enum_t
		{
			Unknown				= 0,
			Unreacheble			= 1,
			SessionRequested	= 2,
			SessionEstablished	= 3
		};
	};

	//! Ctor
	CPingHost
	(
		IKernel& kernel, 
		ILog& log, 
		const std::string& localGuid,
		const std::string& remoteGuid,
		const std::string& localIp,
		const std::string& localPort,
		const std::size_t pingInterval
	);

	~CPingHost(void);

	//! Set direct ep
	void	DirectEndpoint(const std::string& ip, const std::string& port);

	//! Set NAT ep
	void	NATEndpoint(const std::string& ip, const std::string& port);

	//! Ping host endpoints
	void	Ping();

	//! Handle ping request packet
	void	HandleRequest(const ProtoPacketPtr packet);

	//! Set ping interval
	void	PingInterval(const std::size_t interval);

	//! Set incoming status
	void	IncomingStatus(const Status::Enum_t status);

	//! Set outgoing status
	void	OutgoingStatus(const Status::Enum_t status);

private:

	//! Implementation
	class Impl;
	std::auto_ptr<Impl> m_pImpl;
};
#endif // Host_h__
