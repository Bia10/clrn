#include "StdAfx.h"
#include "Procedure.h"

//! Scripts stream helper
template<typename T>
std::basic_ostream<T>& operator << (std::basic_ostream<T>& out, const CProcedure::Scripts& scripts) 
{
	BOOST_FOREACH(const std::string& sql, scripts)
		out << sql << std::endl;
	return out;
}

CProcedure::CProcedure(IKernel& kernel)
	: m_Kernel(kernel)
{
}

CProcedure::~CProcedure(void)
{
}

ProtoTablePtr CProcedure::Execute(const boost::format& sql, const std::string& host /*= ""*/)
{
	return Execute(sql.str(), host);
}

void CProcedure::Execute(const boost::format& sql, const IJob::CallBackFn& callback, const std::string& host /*= ""*/)
{
	Execute(sql.str(), callback, host);
}

void CProcedure::Execute(const Id::Enum_t id, const ParamsMap& procedureParams, const IJob::CallBackFn& callback, const std::string& host /*= ""*/)
{
	// first line: 0 - procedure id
	// second line: param names
	// next line: param values

	TRY 
	{
		IJob::TableList params;
		IJob::TablePtr param(new IJob::AutoPtr(new data::Table()));
		params.push_back(param);

		(*param)->add_rows()->add_data(conv::cast<std::string>(id));	// procedure id in first line
		data::Table_Row* second = (*param)->add_rows();					// procedure param names		
		data::Table_Row* third = (*param)->add_rows();					// procedure param values
		BOOST_FOREACH(const ParamsMap::value_type& pair, procedureParams)
		{
			second->add_data(pair.first);
			third->add_data(pair.second);
		}

		m_Kernel.ExecuteJob(jobs::Job_JobId_EXEC_PROCEDURE, params, host, callback);
	}
	CATCH_PASS_EXCEPTIONS(id, host)

}

ProtoTablePtr CProcedure::Execute(const Scripts& sql, const std::string& host /*= ""*/)
{
	TRY 
	{
		IJob::TableList params;
		IJob::TablePtr param(new IJob::AutoPtr(new data::Table()));
		params.push_back(param);

		IKernel::TablesList tables;

		BOOST_FOREACH(const std::string& script, sql)
			(*param)->add_rows()->add_data(script);

		m_Kernel.ExecuteJob(jobs::Job_JobId_EXEC_SCRIPT, tables, params, host);

		CHECK(1 == tables.size(), sql, host);

		return tables.front();
	}
	CATCH_PASS_EXCEPTIONS(sql, host)

}

void CProcedure::Execute(const Scripts& sql, const IJob::CallBackFn& callback, const std::string& host /*= ""*/)
{
	TRY 
	{
		IJob::TableList params;
		IJob::TablePtr param(new IJob::AutoPtr(new data::Table()));
		params.push_back(param);

		BOOST_FOREACH(const std::string& script, sql)
			(*param)->add_rows()->add_data(script);

		m_Kernel.ExecuteJob(jobs::Job_JobId_EXEC_SCRIPT, params, host, callback);
	}
	CATCH_PASS_EXCEPTIONS(sql, host)
}

ProtoTablePtr CProcedure::Execute(const std::string& sql, const std::string& host)
{
	TRY 
	{
		IJob::TableList params;
		IJob::TablePtr param(new IJob::AutoPtr(new data::Table()));
		params.push_back(param);
	
		IKernel::TablesList tables;
		(*param)->add_rows()->add_data(sql);
		m_Kernel.ExecuteJob(jobs::Job_JobId_EXEC_SCRIPT, tables, params, host);
	
		CHECK(1 == tables.size(), sql, host);
	
		return tables.front();
	}
	CATCH_PASS_EXCEPTIONS(sql, host)
}

void CProcedure::Execute(const std::string& sql, const IJob::CallBackFn& callback, const std::string& host)
{
	TRY 
	{
		IJob::TableList params;
		IJob::TablePtr param(new IJob::AutoPtr(new data::Table()));
		params.push_back(param);
	
		(*param)->add_rows()->add_data(sql);
		m_Kernel.ExecuteJob(jobs::Job_JobId_EXEC_SCRIPT, params, host, callback);
	}
	CATCH_PASS_EXCEPTIONS(sql, host)
}
