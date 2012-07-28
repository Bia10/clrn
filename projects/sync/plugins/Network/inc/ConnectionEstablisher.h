#ifndef ConnectionEstablisher_h__
#define ConnectionEstablisher_h__

#include <memory>

#include <boost/noncopyable.hpp>

class IKernel;
class ILog;

namespace net
{

//! Connection establishment implementation
//!		Purposes:
//! 1. Monitor remote host events(hosts, host_map) 
//! 2. Monitor local host events(host_status)
//! 3. Analyze ability to connect through NAT
//! 4. SYN packets generation
//! 5. host_map event generation(with NAT endpoint of the remote host)
//!
class CConnectionEstablisher : boost::noncopyable
{
		//! Ctor
	CConnectionEstablisher(IKernel& kernel, ILog& log);
	~CConnectionEstablisher(void);

public:

	//! Create instance
	static void Create(IKernel& kernel, ILog& log);

	//! Shutdown instance
	static void Shutdown();

private:
	//! Implementation
	class Impl;
	std::auto_ptr<Impl> m_pImpl;

	//! Single instance
	static std::auto_ptr<CConnectionEstablisher> s_pInstance;
};

} // namespace net

#endif // ConnectionEstablisher_h__
