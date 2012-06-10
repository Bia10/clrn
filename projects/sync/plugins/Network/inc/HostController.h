#ifndef HostPinger_h__
#define HostPinger_h__

#include <boost/scoped_ptr.hpp>

//! Forward declarations
class ILog;
class IKernel;

//! Host pinger
class CHostController
{
	//! Ctor
	CHostController(ILog& logger, IKernel& kernel);
public:
	
	~CHostController(void);

	//! Create instance
	static void					Create(ILog& logger, IKernel& kernel);

	//! Single instance reference
	static CHostController&			Instance();

	//! Close instance
	static void					Shutdown();

	//! On ping received
	void						OnRemotePingReceived(const ProtoPacketPtr packet);

private:

	//! Implementation
	class Impl;
	boost::scoped_ptr<Impl>					m_pImpl;

	//! Single instance
	static boost::scoped_ptr<CHostController>	s_pInstance;
};
#endif // HostPinger_h__
