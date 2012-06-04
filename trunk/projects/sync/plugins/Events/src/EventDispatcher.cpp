#include "StdAfx.h"
#include "EventDispatcher.h"


#include <limits>

//! Event dispatcher implementation
//!
//! \class CEventDispatcher::Impl
//!
class CEventDispatcher::Impl
{
	void operator = (const Impl&);

	//! Event signal type
	typedef boost::signal<void (const ProtoPacketPtr)>	EventSignal;

	//! Signal pointer
	typedef boost::shared_ptr<EventSignal>				EventSignalPtr;

	//! Events map type
	typedef std::map<std::string, EventSignalPtr>		EventsMap;


public:

	Impl(ILog& logger, IKernel& kernel)
		: m_Log(logger)
		, m_Kernel(kernel)
	{
		SCOPED_LOG(m_Log);
	}

	~Impl()
	{
		SCOPED_LOG(m_Log);
	}

	//! Subscribe to event
	void Subscribe(const std::string& name, const IJob::CallBackFn& callBack)
	{
		SCOPED_LOG(m_Log);

		LOG_TRACE("Subscribing event: [%s].") % name;

		TRY 
		{
			boost::mutex::scoped_lock lock(m_EventsMutex);

			// find or create new signal
			EventsMap::iterator it = m_Events.find(name);
			if (m_Events.end() == it)
			{
				const EventsMap::value_type event = std::make_pair( name, EventSignalPtr(new EventSignal()) );
				it = m_Events.insert(event).first;
			}

			// connect callback to slot
			EventSignal& signal = *it->second;
			signal.connect(callBack);
		}
		CATCH_PASS_EXCEPTIONS("Subscribe failed.")
	}

	//! Signal event
	void Signal(const std::string& name, const ProtoPacketPtr packet)
	{
		SCOPED_LOG(m_Log);

		LOG_TRACE("Signaling event: name: [%s], data: [%s].") % name % packet->DebugString();

		TRY 
		{
			// find event
			const EventsMap::const_iterator it = m_Events.find(name);
			if (m_Events.end() == it)
			{
				LOG_WARNING("Event: name: [%s], data: [%s], don't have any subscribers, ignored.") % name % packet->DebugString();
				return;
			}

			(*it->second)(packet);
		}
		CATCH_PASS_EXCEPTIONS("Signal failed.")
	}

private:

	//! Logger
	ILog&					m_Log;

	//! Kernel
	IKernel&				m_Kernel;

	//! Registered events
	EventsMap				m_Events;

	//! Events mutex
	boost::mutex			m_EventsMutex;
};


boost::scoped_ptr<CEventDispatcher> CEventDispatcher::s_pInstance;

CEventDispatcher::CEventDispatcher(ILog& logger, IKernel& kernel)
	: m_pImpl(new Impl(logger, kernel))
{

}

CEventDispatcher::~CEventDispatcher(void)
{
}

void CEventDispatcher::Create(ILog& logger, IKernel& kernel)
{
	s_pInstance.reset(new CEventDispatcher(logger, kernel));
}

CEventDispatcher& CEventDispatcher::Instance()
{
	return *s_pInstance;
}

void CEventDispatcher::Shutdown()
{
	s_pInstance.reset();
}

void CEventDispatcher::Subscribe(const std::string& name, const IJob::CallBackFn& callBack)
{
	m_pImpl->Subscribe(name, callBack);
}

void CEventDispatcher::Signal(const std::string& name, const ProtoPacketPtr packet)
{
	m_pImpl->Signal(name, packet);
}
