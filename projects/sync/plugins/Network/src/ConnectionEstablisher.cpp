#include "stdafx.h"
#include "ConnectionEstablisher.h"
#include "UDPHost.h"

namespace net
{

//! Establisher implementation
class CConnectionEstablisher::Impl : boost::noncopyable
{
	//! Hosts descriptors type
	typedef std::map<std::string, CUDPHost::Status::Enum_t>			Hosts;

	//! Host ping mapping
	typedef std::map<std::size_t, std::string>						HostsPingMap;

	//! Remote hosts mapping type (mapping is: <host_guid> : <reachable from list>)
	typedef std::map<std::string, HostsPingMap>						RemoteHostsMapping;

	//! Host connect timeout ratio
	enum															{HOST_CONNECT_TIMEOUT_RATIO = 4};

public:

	//! Ctor
	Impl(IKernel& kernel, ILog& log)
		: m_Kernel(kernel)
		, m_Log(log)
		, m_LocalHostGuid(kernel.Settings().LocalGuid())
		, m_PingInterval(kernel.Settings().PingInterval())
	{
		SCOPED_LOG(m_Log);

		// subscribe to host status event
		CEvent hostStatusEvent(m_Kernel, HOST_STATUS_EVENT_NAME);
		hostStatusEvent.Subscribe(boost::bind(&Impl::LocalHostStatusCallBack, this, _1));

		// subscribing to host check event
		m_Kernel.Timer(boost::posix_time::milliseconds(m_PingInterval * HOST_CONNECT_TIMEOUT_RATIO), boost::bind(&Impl::HostControlTimeEvent, this));
	}

	//! Dtor
	~Impl()
	{
		SCOPED_LOG(m_Log);
	}

	//! Hosts status event callback(local)
	void LocalHostStatusCallBack(const ProtoPacketPtr packet)
	{
		SCOPED_LOG(m_Log);

		CHECK(packet);

		TRACE_PACKET(packet);

		TRY 
		{
			const data::Table& table = packet->job().results(0);
			const std::string& guid = table.rows(0).data(0);

			LOG_TRACE("Catched host status event, action: [%s]. Guid: [%s]") % table.action() % guid;
	
			if (data::Table_Action_Insert == table.action())
			{
				// subscribe to events on this hosts
				CEvent remoteHostsEvent(m_Kernel, HOSTS_TABLE_NAME);
				remoteHostsEvent.Subscribe(boost::bind(&Impl::RemoteHostsCallBack, this, _1), guid);
	
				CEvent remoteHostMapEvent(m_Kernel, HOSTMAP_EVENT_NAME);
				remoteHostMapEvent.Subscribe(boost::bind(&Impl::RemoteHostMapCallBack, this, _1, guid), guid);		
	
				// registering this host data
				boost::mutex ::scoped_lock lock(m_HostsMutex);
				m_HostStatuses[guid] = CUDPHost::Status::SessionEstablished;
			}
			else
			if (data::Table_Action_Delete == table.action())
			{
				// registering this host data
				boost::mutex ::scoped_lock lock(m_HostsMutex);
				m_HostStatuses[guid] = CUDPHost::Status::Unreacheble;
			}
		}
		CATCH_PASS_EXCEPTIONS(*packet)
	}

	//! Remote hosts callback
	void RemoteHostsCallBack(const ProtoPacketPtr packet)
	{
		SCOPED_LOG(m_Log);

		if (!packet)
			return; // host went offline

		TRACE_PACKET(packet);

		TRY 
		{
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
		CATCH_PASS_EXCEPTIONS(*packet)
	}

	//! Remote host_map callback
	void RemoteHostMapCallBack(const ProtoPacketPtr packet, const std::string& hostGuid)
	{
		SCOPED_LOG(m_Log);

		if (!packet)
			return; // host went offline

		TRY 
		{
			// host map params
			CTable table(*packet->mutable_job()->mutable_results(0));	
	
			BOOST_FOREACH(const CTable::Row& row, table)
			{
				const CUDPHost::Status::Enum_t status = static_cast<CUDPHost::Status::Enum_t>(conv::cast<std::size_t>(row["status"]));
				const std::size_t ping = conv::cast<std::size_t>(row["ping"]);
	
				// getting host map params
				const std::string& from = row["from"];
				const std::string& to = row["to"];
	
				if (CUDPHost::Status::SessionEstablished == status)
				{
					// processing only direct connections
					boost::mutex::scoped_lock lock(m_HostMapMutex);
					if (from == hostGuid)
						m_RemoteMapping[to][ping] = from;
					else
					if (to == hostGuid)
						m_RemoteMapping[from][ping] = to;
				}
				else
				if (CUDPHost::Status::SessionRequested == status && to == m_LocalHostGuid)
				{
					// process incoming request
					// it's mean that someone want to connect with us, 
					// and he sended CONNECT job to 'hostGuid', then server 'hostGuid'
					// generated 'host_map' record and event, which we caught here.
	
					// now in received host_map event record we got remote host(which wants to connect with us)
					// endpoint. to connect to this server need to UDPHost know about this endpoint,
					// so we need to generate host map event with this record to inform Kernel, UDPServer and then UDPHost about this
	
					// signal host_map event with incoming session request params
					CProcedure script(m_Kernel);
					CProcedure::ParamsMap mapParams;
	
					mapParams["from"]		= hostGuid;
					mapParams["to"]			= m_LocalHostGuid;
					mapParams["status"]		= conv::cast<std::string>(CUDPHost::Status::SessionEstablished);
					mapParams["ping"]		= conv::cast<std::string>(std::numeric_limits<std::size_t>::max());
					mapParams["ip"]			= row["ip"];
					mapParams["port"]		= row["port"];	
	
					script.Execute(CProcedure::Id::HostMapCreate, mapParams, IJob::CallBackFn());				
				}
			}
		}
		CATCH_PASS_EXCEPTIONS(packet, hostGuid)
	}

	//! Host control time event
	void HostControlTimeEvent()
	{
		SCOPED_LOG(m_Log);

		m_Kernel.Timer(boost::posix_time::milliseconds(m_PingInterval * HOST_CONNECT_TIMEOUT_RATIO), boost::bind(&Impl::HostControlTimeEvent, this));

		// check hosts, find unavailable
		boost::mutex::scoped_lock lock(m_HostsMutex);
		
		BOOST_FOREACH(const Hosts::value_type& pair, m_HostStatuses)
		{
			if (CUDPHost::Status::Unreacheble != pair.second)
				continue;

			// this host is unreachable, check remote host mapping
			boost::mutex::scoped_lock lock(m_HostMapMutex);
			const HostsPingMap& hosts = m_RemoteMapping[pair.first];

			if (hosts.empty())
				continue;

			// host from which this host is reachable
			const std::string& server = hosts.begin()->second;
			
			// sending 'CONNECT'
			SendConnect(server, pair.first);
		}
	}

	//! Send connect job
	void SendConnect(const std::string& server, const std::string& target)
	{
		SCOPED_LOG(m_Log);

		TRY 
		{
			IJob::TableList params;
			IJob::TablePtr param(new IJob::AutoPtr(new data::Table()));
			params.push_back(param);
		
			data::Table_Row* tableRow = (*param)->add_rows();
			tableRow->add_data(target); // target host
		
			m_Kernel.ExecuteJob
			(
				jobs::Job_JobId_CONNECT, 
				params, 
				server, 
				IJob::CallBackFn()
			);
		}
		CATCH_PASS_EXCEPTIONS(server, target)
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
	Hosts				m_HostStatuses;

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
	ILog& log
)
	: m_pImpl(new Impl(kernel, log))
{
}


CConnectionEstablisher::~CConnectionEstablisher(void)
{
}

} // namespace net