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
	Impl(IKernel& kernel, ILog& log, const std::string& localHostGuid, const std::size_t pingInterval)
		: m_Kernel(kernel)
		, m_Log(log)
		, m_LocalHostGuid(localHostGuid)
		, m_PingInterval(pingInterval)
	{
		// subscribe to host status event
		CEvent hostStatusEvent(m_Kernel, HOST_STATUS_EVENT_NAME);
		hostStatusEvent.Subscribe(boost::bind(&Impl::LocalHostStatusCallBack, this, _1));

		// subscribe to time event
		m_Kernel.TimeEvent(boost::posix_time::milliseconds(m_PingInterval), boost::bind(&Impl::HostControlTimeEvent, this), true);
	}

	//! Hosts status event call back(local)
	void LocalHostStatusCallBack(const ProtoPacketPtr packet)
	{
		CHECK(packet);

		const data::Table& table = packet->job().results(0);
		const std::string& guid = table.rows(0).data(0);

		if (data::Table_Action_Insert == table.action())
		{
			// subscribe to events on this hosts
			CEvent remoteHostsEvent(m_Kernel, HOSTS_TABLE_NAME);
			remoteHostsEvent.Subscribe(boost::bind(&Impl::RemoteHostsCallBack, this, _1), guid);

			CEvent remoteHostMapEvent(m_Kernel, HOSTMAP_EVENT_NAME);
			remoteHostMapEvent.Subscribe(boost::bind(&Impl::RemoteHostMapCallBack, this, _1, guid), guid);		

			// registering this host data
			boost::mutex ::scoped_lock lock(m_HostsMutex);
			m_Hosts[guid].status = CPingHost::Status::SessionEstablished;
		}
		else
		if (data::Table_Action_Delete == table.action())
		{
			// registering this host data
			boost::mutex ::scoped_lock lock(m_HostsMutex);
			m_Hosts[guid].status = CPingHost::Status::Unreacheble;
			m_Hosts[guid].wentOffline = boost::posix_time::microsec_clock::local_time();
		}
	}

	//! Remote hosts callback
	void RemoteHostsCallBack(const ProtoPacketPtr packet)
	{
		CHECK(packet);

		// host params
		CTable table(*packet->mutable_job()->mutable_results(0));	

		BOOST_FOREACH(const CTable::Row& row, table)
		{
			// add this host through procedure
			CProcedure proc(m_Kernel);
			CProcedure::ParamsMap params;
			params["guid"]	= row["guid"];
			params["ip"]	= row["ip"];
			params["port"]	= row["port"];
			proc.Execute(CProcedure::Id::HostsCreate, params, IJob::CallBackFn());
		}
	}

	//! Remote host_map callback
	void RemoteHostMapCallBack(const ProtoPacketPtr packet, const std::string& hostGuid)
	{
		CHECK(packet);

		// host map params
		CTable table(*packet->mutable_job()->mutable_results(0));	

		BOOST_FOREACH(const CTable::Row& row, table)
		{
			const CPingHost::Status::Enum_t status = static_cast<CPingHost::Status::Enum_t>(conv::cast<std::size_t>(row["status"]));

			// getting host map params
			const std::string& from = row["from"];
			const std::string& to = row["to"];

			if (CPingHost::Status::SessionEstablished == status)
			{
				// processing only direct connections
				boost::mutex::scoped_lock lock(m_HostMapMutex);
				if (from == hostGuid)
					m_RemoteMapping[from] = to;
				else
				if (to == hostGuid)
					m_RemoteMapping[to] = from;
			}
			else
			if (CPingHost::Status::SessionRequested == status)
			{
				// process incoming request
				if (to == m_LocalHostGuid)
				{
					// signal host_map event with incoming session request params here
				}
			}
		}
	}

	//! Host control time event
	void HostControlTimeEvent()
	{

	}

private:

	//! Kernel reference
	IKernel&			m_Kernel;

	//! Logger 
	ILog&				m_Log;

	//! Local host guid
	const std::string	m_LocalHostGuid;

	//! Ping interval
	const std::size_t	m_PingInterval;

	//! Hosts
	Hosts				m_Hosts;

	//! Remote hosts mapping
	RemoteHostsMapping	m_RemoteMapping;

	//! Hosts mutex
	boost::mutex		m_HostsMutex;

	//! Host map mutex
	boost::mutex		m_HostMapMutex;

};


CConnectionEstablisher::CConnectionEstablisher
(
	IKernel& kernel, 
	ILog& log, 
	const std::string& localHostGuid, 
	const std::size_t pingInterval
)
	: m_pImpl(new Impl(kernel, log, localHostGuid, pingInterval))
{
}


CConnectionEstablisher::~CConnectionEstablisher(void)
{
}
