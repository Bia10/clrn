#include "stdafx.h"
#include "ConnectionEstablisher.h"
#include "PingHost.h"

//! Establisher implementation
class CConnectionEstablisher::Impl : boost::noncopyable
{
	//! Host descriptor
	struct Host
	{
		boost::posix_time::ptime	wentOffline;
		CPingHost::Status::Enum_t	status;
	};

	//! Hosts descriptors type
	typedef std::map<std::string, Host>			Hosts;

	//! Remote hosts mapping type
	typedef std::map<std::string, std::string>	RemoteHostsMapping;

public:

	//! Ctor
	Impl(IKernel& kernel, ILog& log)
		: m_Kernel(kernel)
		, m_Log(log)
	{
		// subscribe to host status event
		CEvent hostStatusEvent(m_Kernel, HOST_STATUS_EVENT_NAME);
		hostStatusEvent.Subscribe(boost::bind(&Impl::LocalHostStatusCallBack, this, _1));
	}

	//! Hosts status event call back(local)
	void LocalHostStatusCallBack(const ProtoPacketPtr packet)
	{
		const data::Table& table = packet->job().results(0);
		const std::string& guid = table.rows(0).data(0);

		if (data::Table_Action_Insert == table.action())
		{
			// subscribe to events on this hosts
			CEvent remoteHostsEvent(m_Kernel, HOSTS_TABLE_NAME);
			remoteHostsEvent.Subscribe(boost::bind(&Impl::RemoteHostsCallBack, this, _1), guid);

			CEvent remoteHostMapEvent(m_Kernel, HOSTMAP_TABLE_NAME);
			remoteHostMapEvent.Subscribe(boost::bind(&Impl::RemoteHostMapCallBack, this, _1), guid);		
		}
		else
		if (data::Table_Action_Delete == table.action())
		{

		}
	}

	//! Remote hosts callback
	void RemoteHostsCallBack(const ProtoPacketPtr packet)
	{

	}

	//! Remote host_map callback
	void RemoteHostMapCallBack(const ProtoPacketPtr packet)
	{

	}


private:

	//! Kernel reference
	IKernel&			m_Kernel;

	//! Logger 
	ILog&				m_Log;

	//! Hosts
	Hosts				m_Hosts;

	//! Remote hosts mapping
	RemoteHostsMapping	m_RemoteMapping;

};


CConnectionEstablisher::CConnectionEstablisher(IKernel& kernel, ILog& log)
	: m_pImpl(new Impl(kernel, log))
{
}


CConnectionEstablisher::~CConnectionEstablisher(void)
{
}
