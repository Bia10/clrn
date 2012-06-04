#ifndef EventDispatcher_h__
#define EventDispatcher_h__

#include "IJob.h"
#include "Event.h"
#include "ProtoPacketPtr.h"

#include <boost/scoped_ptr.hpp>

// Forward declarations
class ILog;
class IKernel;

//! Class for events registering/dispatching
//!
//! \class CHostsController
//!
class CEventDispatcher
{
	CEventDispatcher(ILog& logger, IKernel& kernel);
public:
	~CEventDispatcher(void);

	//! Create instance
	static void					Create(ILog& logger, IKernel& kernel);

	//! Single instance reference
	static CEventDispatcher&	Instance();

	//! Close instance
	static void					Shutdown();

	//! Subscribe to event
	void						Subscribe
	(
		const std::string& name,
		const IJob::CallBackFn& callBack
	);

	//! Signal event
	void						Signal
	(
		const std::string& name,
		const ProtoPacketPtr packet
	);

private:

	//! Implementation
	class Impl;
	boost::scoped_ptr<Impl> m_pImpl;

	//! Single instance
	static boost::scoped_ptr<CEventDispatcher> s_pInstance;

};

#endif // EventDispatcher_h__
