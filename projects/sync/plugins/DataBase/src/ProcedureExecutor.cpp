#include "StdAfx.h"
#include "ProcedureExecutor.h"

//! Params stream helper
std::ostream& operator << (std::ostream& stream, const CProcedure::ParamsMap& params)
{
	BOOST_FOREACH(const CProcedure::ParamsMap::value_type& pair, params)
	{
		stream << "\t[" << pair.first << "] : [" << pair.second << "]" << std::endl;
	}
	return stream;
}

//! Executor implementation
//!
//! \class CProcedureExecutor::Impl
//!
class CProcedureExecutor::Impl : boost::noncopyable
{
	//! Procedure function type
	typedef boost::function<void(const CProcedure::ParamsMap&, data::Table&)>	FunctionType;

	//! Procedure storage type
	typedef std::vector<FunctionType>											ProceduresVector;

public:

	//! Ctor
	Impl(IKernel& kernel)
		: m_Kernel(kernel)
	{
		m_Procedures.resize(CProcedure::Id::ProceduresSize);

		m_Procedures[CProcedure::Id::HostsLoad]				= boost::bind(&Impl::HostsLoad, this, _1, _2);
		m_Procedures[CProcedure::Id::HostsCreate]			= boost::bind(&Impl::HostsCreate, this, _1, _2);
		m_Procedures[CProcedure::Id::HostsDelete]			= boost::bind(&Impl::HostsDelete, this, _1, _2);

		m_Procedures[CProcedure::Id::HostMapLoad]			= boost::bind(&Impl::HostMapLoad, this, _1, _2);
		m_Procedures[CProcedure::Id::HostMapCreate]			= boost::bind(&Impl::HostMapCreate, this, _1, _2);
		m_Procedures[CProcedure::Id::HostMapDelete]			= boost::bind(&Impl::HostMapDelete, this, _1, _2);
	}

	//! Get parameter
	const std::string& GetParam(const CProcedure::ParamsMap& params, const std::string& name)
	{
		const CProcedure::ParamsMap::const_iterator it = params.find(name);
		CHECK(params.end() != it, name);
		return it->second;
	}

	//! Execute procedure
	void Execute(const CProcedure::Id::Enum_t id, const CProcedure::ParamsMap& params, data::Table& result)
	{
		TRY 
		{
			const std::size_t index = static_cast<std::size_t>(id);
			CHECK(index < m_Procedures.size(), id, m_Procedures.size());
	
			m_Procedures[index](params, result);
		}
		CATCH_PASS_EXCEPTIONS(params)
	}

	//! Load hosts
	void HostsLoad(const CProcedure::ParamsMap& /*params*/, data::Table& result)
	{
		DataBase::Instance().Execute("select * from hosts", result, data::Table_Id_Hosts);
		result.set_action(data::Table_Action_Insert);
	}

	//! Create host procedure
	void HostsCreate(const CProcedure::ParamsMap& params, data::Table& /*result*/)
	{
		const std::string& guid		= GetParam(params, "guid");
		const std::string& ip		= GetParam(params, "ip");;
		const std::string& port		= GetParam(params, "port");

		std::string sql = "insert into hosts(guid, ip, port) values(':GUID', ':IP', :PORT)";
		boost::algorithm::replace_all(sql, ":GUID", guid);
		boost::algorithm::replace_all(sql, ":IP", ip);
		boost::algorithm::replace_all(sql, ":PORT", port);

		if ( !DataBase::Instance().Execute(sql.c_str()) )
			return;
		
		sql = "select * from hosts where guid = ':GUID'";
		boost::algorithm::replace_all(sql, ":GUID", guid);

		// event packet
		const ProtoPacketPtr packet(new packets::Packet());
		data::Table* result = packet->mutable_job()->add_results();

		DataBase::Instance().Execute(sql.c_str(), *result, data::Table_Id_Hosts);
		result->set_action(data::Table_Action_Insert);

		// signal event
		CEvent evnt(m_Kernel, HOSTS_TABLE_NAME);
		evnt.Signal(packet);
	}

	//! Hosts delete
	void HostsDelete(const CProcedure::ParamsMap& params, data::Table& /*result*/)
	{
		const std::string& guid	= GetParam(params, "guid");

		std::string sql = "select * from hosts where guid = ':GUID'";
		boost::algorithm::replace_all(sql, ":GUID", guid);

		// event packet
		const ProtoPacketPtr packet(new packets::Packet());
		data::Table* result = packet->mutable_job()->add_results();

		DataBase::Instance().Execute(sql.c_str(), *result, data::Table_Id_HostMap);
		result->set_action(data::Table_Action_Delete);

		sql = "delete from hosts where guid = ':GUID'";
		boost::algorithm::replace_all(sql, ":GUID", guid);

		if ( !DataBase::Instance().Execute(sql.c_str()) )
			return;

		// signal event
		CEvent evnt(m_Kernel, HOSTS_TABLE_NAME);
		evnt.Signal(packet);
	}

	//! Host map load
	void HostMapLoad(const CProcedure::ParamsMap& /*params*/, data::Table& result)
	{
		static const char* sql = 
			"SELECT h2.[guid], "
			"	   h1.[guid],  "
			"	   hm.status,  "
			"	   hm.ping,    "
			"      hm.ip,      "
			"      hm.port     "
			"FROM   host_map hm  "
			"	   LEFT JOIN hosts h1  "
			"			  ON h1.[id] = hm.dst  "
			"	   LEFT JOIN hosts h2  "
			"			  ON h2.[id] = hm.src  ";

		DataBase::Instance().Execute(sql, result, data::Table_Id_HostMapEvent);
		result.set_action(data::Table_Action_Insert);
	}

	//! Host map create
	void HostMapCreate(const CProcedure::ParamsMap& params, data::Table& /*result*/)
	{
		const std::string& guid		= GetParam(params, "guid");
		const std::string& ip		= GetParam(params, "ip");;
		const std::string& port		= GetParam(params, "port");	
		const std::string& status	= GetParam(params, "status");	
		const std::string& ping		= GetParam(params, "ping");	
		const bool incoming			= conv::cast<bool>(GetParam(params, "incoming"));	

		std::string sql;
		if (!incoming)
			sql = "insert into host_map select 1, id, :STATUS, :PING, :IP, :PORT from hosts where guid = ':GUID'";
		else
			sql = "insert into host_map select id, 1, :STATUS, :PING, :IP, :PORT from hosts where guid = ':GUID'";

		boost::algorithm::replace_all(sql, ":STATUS",	status);
		boost::algorithm::replace_all(sql, ":PING",		ping);
		boost::algorithm::replace_all(sql, ":IP",		!ip.empty() ? (std::string("'") + ip + "'") : "NULL");
		boost::algorithm::replace_all(sql, ":PORT",		port);
		boost::algorithm::replace_all(sql, ":GUID",		guid);

		// inserting data
		if ( !DataBase::Instance().Execute(sql.c_str()) )
			return;

		// selecting data and raising event
		data::Table temp;
		DataBase::Instance().Execute("select guid from hosts where id = 1", temp);
		const std::string& localGuid = temp.rows(0).data(0);

		// event packet
		const ProtoPacketPtr packet(new packets::Packet());
		data::Table* result = packet->mutable_job()->add_results();
		result->set_action(data::Table_Action_Insert);
		result->set_id(data::Table_Id_HostMapEvent);

		CTable resultTable(*result);

		CTable::Row& row = resultTable.AddRow();

		if (incoming)
		{
			row["from"]		= guid;
			row["to"]		= localGuid;
		}
		else
		{
			row["from"]		= localGuid;
			row["to"]		= guid;
		}
		row["status"]	= status;
		row["ping"]		= ping;
		row["ip"]		= ip;
		row["port"]		= port;	

		// signal event
		CEvent evnt(m_Kernel, HOSTMAP_EVENT_NAME);
		evnt.Signal(packet);
	}

	//! Host map delete
	void HostMapDelete(const CProcedure::ParamsMap& params, data::Table& /*result*/)
	{
		const std::string& from	= GetParam(params, "from");
		const std::string& to	= GetParam(params, "to");

		std::string sql =
			"SELECT h2.[guid], " 
			"	   h1.[guid],  "
			"	   hm.status,  "
			"	   hm.ping,    "
			"	   hm.ip,      "
			"      hm.port	   "
			"FROM   host_map hm  "
			"	   LEFT JOIN hosts h1  "
			"			  ON h1.[id] = hm.dst  "
			"	   LEFT JOIN hosts h2  "
			"			  ON h2.[id] = hm.src  "
			"	   WHERE h2.[guid] = ':FROM'	"
			"	   AND   h1.[guid] = ':TO'";

		boost::algorithm::replace_all(sql, ":FROM",	from);
		boost::algorithm::replace_all(sql, ":TO",	to);

		// event packet
		const ProtoPacketPtr packet(new packets::Packet());
		data::Table* result = packet->mutable_job()->add_results();
		result->set_action(data::Table_Action_Delete);

		// selecting data
		DataBase::Instance().Execute(sql.c_str(), *result, data::Table_Id_HostMapEvent);

		if (!result->rows_size())
			return; // this host doesn't exists

		// deleting data
		sql = 
			"DELETE FROM host_map "
			"WHERE  src IN (SELECT id  "
			"			   FROM   hosts  "
			"			   WHERE  guid = ':FROM')  "
			"	   AND dst IN (SELECT id  "
			"				   FROM   hosts " 
			"				   WHERE  guid = ':TO') ";

		boost::algorithm::replace_all(sql, ":FROM",	from);
		boost::algorithm::replace_all(sql, ":TO",	to);

		// delete data
		CHECK(DataBase::Instance().Execute(sql.c_str()));

		// signal event
		CEvent evnt(m_Kernel, HOSTMAP_EVENT_NAME);
		evnt.Signal(packet);
	}


private:

	//! Procedures
	ProceduresVector		m_Procedures;

	//! Kernel reference
	IKernel&				m_Kernel;
};

CProcedureExecutor::CProcedureExecutor(IKernel& kernel)
	: m_pImpl(new Impl(kernel))
{
}

CProcedureExecutor::~CProcedureExecutor(void)
{

}

CProcedureExecutor& CProcedureExecutor::Instance(IKernel& kernel)
{
	static CProcedureExecutor executor(kernel);
	return executor;
}

void CProcedureExecutor::Execute(const CProcedure::Id::Enum_t id, const CProcedure::ParamsMap& params, data::Table& result)
{
	m_pImpl->Execute(id, params, result);
}
