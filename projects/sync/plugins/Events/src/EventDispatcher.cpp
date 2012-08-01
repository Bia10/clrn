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
		std::string			name;
		std::string			host;
		std::string			hash;
	};

	//! Callbacks
	typedef std::list<CallbackInfo>	Callbacks;

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
	std::string Subscribe(const std::string& name, const std::string& host, const std::string& caller, const IJob::CallBackFn& callBack)
	{
		SCOPED_LOG(m_Log);

		LOG_TRACE("Subscribing to event: [%s], host: [%s], caller: [%s].") % name % host % caller;

		TRY 
		{
			// generate hash
			const std::string hash = GetHash(name, host, caller);

			boost::recursive_mutex::scoped_lock lock(m_CallbackMutex);

			// check for this hash existence
			const Callbacks::const_iterator it = std::find_if
			(
				m_Callbacks.begin(),
				m_Callbacks.end(),
				boost::bind(&CallbackInfo::hash, _1) == hash
			);

			if (m_Callbacks.end() != it)
			{
				// unsubscribe all host callbacks
				UnSubscribeHost(host);

				// signal status event
				SignalStatusEvent(false, host);
			}

			// simply add new callback
			CallbackInfo info;
			info.callback = callBack;
			info.hash = hash;
			info.host = host;
			info.name = name;

			m_Callbacks.push_back(info);

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
			boost::recursive_mutex::scoped_lock lock(m_CallbackMutex);

			// find callbacks and invoke them
			BOOST_FOREACH(const CallbackInfo& info, m_Callbacks)
			{
				if (info.name == name)
					info.callback(packet);
			}
		}
		CATCH_PASS_EXCEPTIONS("Signal failed.", name, *packet)
	}

	//! Unsubscribe
	void UnSubscribe(const std::string& hash)
	{
		SCOPED_LOG(m_Log);

		LOG_TRACE("Unsubscribing from event hash: [%s].") % hash;

		TRY 
		{
			boost::recursive_mutex::scoped_lock lock(m_CallbackMutex);

			// find callback and erase it
			const Callbacks::const_iterator it = std::find_if
			(
				m_Callbacks.begin(),
				m_Callbacks.end(),
				boost::bind(&CallbackInfo::hash, _1) == hash
			);

			CHECK(m_Callbacks.end() != it);

			m_Callbacks.erase(it);
		}
		CATCH_PASS_EXCEPTIONS("UnSubscribe failed.", hash)
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

		boost::recursive_mutex::scoped_lock lock(m_CallbackMutex);

		Callbacks::const_iterator it = m_Callbacks.begin();
		const Callbacks::const_iterator itEnd = m_Callbacks.end();
		for (; it != itEnd; )
		{
			if (it->host == host)
			{
				LOG_WARNING("Unsubscribing event name: [%s], hash: [%s] from host: [%s].") % it->name % it->hash % it->host;
				it = m_Callbacks.erase(it);
			}
			else
				++it;
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

	//! Registered callbacks
	Callbacks				m_Callbacks;

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

void CEventDispatcher::UnSubscribe(const std::string& hash)
{
	m_pImpl->UnSubscribe(hash);
}

std::string CEventDispatcher::Subscribe(const std::string& name, const std::string& host, const std::string& caller, const IJob::CallBackFn& callBack)
{
	return m_pImpl->Subscribe(name, host, caller, callBack);
}
