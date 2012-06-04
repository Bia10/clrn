#ifndef HostPinger_h__
#define HostPinger_h__

#include <boost/scoped_ptr.hpp>

//! Forward declarations
class ILog;
class IKernel;

//! Host pinger
class CHostPinger
{
	//! Ctor
	CHostPinger(ILog& logger, IKernel& kernel);
public:

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
	
	~CHostPinger(void);

	//! Create instance
	static void					Create(ILog& logger, IKernel& kernel);

	//! Single instance reference
	static CHostPinger&			Instance();

	//! Close instance
	static void					Shutdown();

	//! On ping received
	void						OnRemotePingReceived(const ProtoPacketPtr packet);

private:

	//! Implementation
	class Impl;
	boost::scoped_ptr<Impl>					m_pImpl;

	//! Single instance
	static boost::scoped_ptr<CHostPinger>	s_pInstance;
};
#endif // HostPinger_h__
