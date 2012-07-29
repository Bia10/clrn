#include "StdAfx.h"
#include "EventDispatcher.h"


//! Event dispatcher implementation
//!
//! \class CEventDispatcher::Impl
//!
class CEventDispatcher::Impl
{
	void operator = (const Impl&);

	//! Callback info
	struct CallbackInfo
	{
		IJob::CallBackFn	callback;
		std::string			host;
	};

	//! Signals map
	typedef std::map<std::size_t, CallbackInfo>		SignalsMap;

	//! Events map type
	typedef std::map<std::string, SignalsMap>		EventsMap;

	//! Callback info
	struct Info
	{
		std::size_t index;
		std::string	host;
	};

	//! Callbacks indexes map
	typedef std::map<std::string, Info>				CallbackInfoMap;

public:

	Impl(ILog& logger, IKernel& kernel)
		: m_Log(logger)
		, m_Kernel(kernel)
		, m_CurrentCallbackIndex(0)
	{
		SCOPED_LOG(m_Log);
	}

	~Impl()
	{
		SCOPED_LOG(m_Log);
	}

	//! Subscribe to event
	std::string Subscribe(const std::string& name, const std::string& host, const std::string& caller, const IJob::CallBackFn& callBack)
	{
		SCOPED_LOG(m_Log);

		LOG_TRACE("Subscribing to event: [%s], host: [%s], caller: [%s].") % name % host % caller;

		TRY 
		{
			boost::recursive_mutex::scoped_lock lock(m_EventsMutex);

			// find or create new signal
			EventsMap::iterator it = m_Events.find(name);
			if (m_Events.end() == it)
			{
				const EventsMap::value_type event = std::make_pair( name, SignalsMap() );
				it = m_Events.insert(event).first;
			}

			// connect callback to slot
			SignalsMap& signalsMap = it->second;

			boost::recursive_mutex::scoped_lock lockCallback(m_CallbackMutex);
			const std::string hash = GetHash(name, host, caller);

			const CallbackInfoMap::const_iterator itCallback = m_CallbacksInfo.find(hash);
			if (m_CallbacksInfo.end() != itCallback)
			{
				// unsubscribe all host callbacks
				UnSubscribeHost(host);

				// signal status event
				SignalStatusEvent(false, host);
			}

			CallbackInfo info;
			info.callback = callBack;
			info.host = host;

			signalsMap.insert(std::make_pair(++m_CurrentCallbackIndex, info));			
			m_CallbacksInfo[hash].index = m_CurrentCallbackIndex;
			m_CallbacksInfo[hash].host = host;

			return hash;
		}
		CATCH_PASS_EXCEPTIONS("Subscribe failed.")
	}

	//! Signal event
	void Signal(const std::string& name, const ProtoPacketPtr packet)
	{
		SCOPED_LOG(m_Log);

		CHECK(packet);

		TRACE_PACKET(packet);
		LOG_TRACE("Signaling event: name: [%s], data: [%s]") % name % packet->ShortDebugString();
		 
		TRY 
		{
			boost::recursive_mutex::scoped_lock lock(m_EventsMutex);

			// find event
			const EventsMap::const_iterator it = m_Events.find(name);
			if (m_Events.end() == it)
			{
				LOG_WARNING("Event: name: [%s], data: [%s], don't have any subscribers, ignored.") % name % packet->ShortDebugString();
				return;
			}

			const SignalsMap& signalsMap = it->second;
			BOOST_FOREACH(const SignalsMap::value_type& pair, signalsMap)
			{
				pair.second.callback(packet);
			}

		}
		CATCH_PASS_EXCEPTIONS("Signal failed.", name, *packet)
	}

	//! Unsubscribe
	void UnSubscribe(const std::string& name, const std::string& hash)
	{
		SCOPED_LOG(m_Log);

		LOG_TRACE("Unsubscribing from event hash: [%s].") % hash;

		TRY 
		{
			boost::recursive_mutex::scoped_lock lock(m_EventsMutex);

			// find signal
			EventsMap::iterator it = m_Events.find(name);
			if (m_Events.end() == it)
				return;

			boost::recursive_mutex::scoped_lock lockCallback(m_CallbackMutex);

			const CallbackInfoMap::const_iterator itCallback = m_CallbacksInfo.find(hash);
			CHECK(m_CallbacksInfo.end() != itCallback);

			// disconnect callback from slot
			SignalsMap& signalsMap = it->second;

			const SignalsMap::const_iterator itSignal = signalsMap.find(itCallback->second.index);
			CHECK(itSignal != signalsMap.end(), itCallback->second.index);
			
			signalsMap.erase(itSignal);			
			m_CallbacksInfo.erase(itCallback);
		}
		CATCH_PASS_EXCEPTIONS("UnSubscribe failed.", name, hash)
	}


private:

	//! Signal status event
	void SignalStatusEvent(const bool online, const std::string& guid)
	{
		const ProtoPacketPtr eventPacket(new packets::Packet());
		TRACE_PACKET(eventPacket, online);
		data::Table& resultTable = *eventPacket->mutable_job()->add_results();
		resultTable.set_action(online ? data::Table_Action_Insert : data::Table_Action_Delete);
		resultTable.set_id(data::Table_Id_HostStatusEvent);

		CTable table(resultTable);
		table.AddRow()["guid"] = guid;

		CEvent evnt(m_Kernel, HOST_STATUS_EVENT_NAME);
		const IJob::CallBackFn callback(!online ? boost::bind(&Impl::HostOnlineCallBack, this, _1, guid) : IJob::CallBackFn());
		evnt.Signal(eventPacket, callback);
	}

	//! Host offline event signal callback
	void HostOnlineCallBack(const ProtoPacketPtr packet, const std::string& guid)
	{
		if (!packet)
			return; // callback failed

		SignalStatusEvent(true, guid);
	}

	//! Unsubscribe host
	void UnSubscribeHost(const std::string& host)
	{
		SCOPED_LOG(m_Log);

		{
			std::vector<CallbackInfoMap::const_iterator> toDelete;

			boost::recursive_mutex::scoped_lock lock(m_CallbackMutex);

			CallbackInfoMap::const_iterator it = m_CallbacksInfo.begin();
			const CallbackInfoMap::const_iterator itEnd = m_CallbacksInfo.end();
			for (; it != itEnd; ++it)
			{
				if (it->second.host == host)
					toDelete.push_back(it);
			}

			LOG_WARNING("Unsubscribing all events: [%s] from host: [%s].") % toDelete.size() % host;

			BOOST_FOREACH(const CallbackInfoMap::const_iterator it, toDelete)
			{
				m_CallbacksInfo.erase(it);
			}
		}
		{
			BOOST_FOREACH(EventsMap::value_type& evnt, m_Events)
			{
				std::vector<std::size_t> toDelete;
				BOOST_FOREACH(const SignalsMap::value_type& signal, evnt.second)
				{
					if (signal.second.host == host)
						toDelete.push_back(signal.first);
				}

				BOOST_FOREACH(const std::size_t index, toDelete)
				{
					evnt.second.erase(index);
				}
			}
		}
	}

	//! Generate hash
	std::string	GetHash(const std::string& name, const std::string& host, const std::string& caller)
	{
		return (boost::format("[%s][%s][%s]") % name % host % caller).str();
	}

	//! Logger
	ILog&					m_Log;

	//! Kernel
	IKernel&				m_Kernel;

	//! Registered events
	EventsMap				m_Events;

	//! Events mutex
	boost::recursive_mutex	m_EventsMutex;

	//! Callback indexes map
	CallbackInfoMap			m_CallbacksInfo;

	//! Current callback index
	std::size_t				m_CurrentCallbackIndex;

	//! Callback indexes mutes
	boost::recursive_mutex	m_CallbackMutex;
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
	CHECK(s_pInstance.get());
	return *s_pInstance;
}

void CEventDispatcher::Shutdown()
{
	s_pInstance.reset();
}

void CEventDispatcher::Signal(const std::string& name, const ProtoPacketPtr packet)
{
	m_pImpl->Signal(name, packet);
}

void CEventDispatcher::UnSubscribe(const std::string& name, const std::string& hash)
{
	m_pImpl->UnSubscribe(name, hash);
}

std::string CEventDispatcher::Subscribe(const std::string& name, const std::string& host, const std::string& caller, const IJob::CallBackFn& callBack)
{
	return m_pImpl->Subscribe(name, host, caller, callBack);
}
