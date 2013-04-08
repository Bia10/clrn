#include "SQLiteDB.h"
#include "CppSQLite3.h"
#include "Exception.h"
#include "SQLiteStatement.h"

#include <sstream>

namespace sql
{



#define CATCH_PASS_SQLITE_EXCEPTION(message)										\
catch(CppSQLite3Exception& e)														\
{																					\
	std::ostringstream oss;															\
	oss << "SQLite error: [" << e.errorCode() << "] Message: ["						\
	<< e.errorMessage() << "] Sql: [" << message << "]";							\
	throw CExcept(__FILE__, __LINE__, oss.str());				                    \
}

#define CATCH_PASS_SQLITE_EXCEPTIONS(message)										\
	CATCH_PASS_SQLITE_EXCEPTION(message)											\
	CATCH_PASS_EXCEPTIONS(message)					

//! SQLite database wrapper implementation
//!
//! \class CSQLiteDB
//! 
class SQLiteDataBase::Impl
{
public:

	Impl(ILog& logger)
		: m_Log(logger)
	{
	}	

	~Impl()
	{
		Close();
	}

	IStatement::Ptr CreateStatement(const std::string& sql)
	{
		TRY 
		{
			SCOPED_LOG(m_Log);

 			LOG_TRACE("Compiling sql statement: [%s].") % sql;

			return IStatement::Ptr(new SQLiteStatement(m_DataBase.compileStatement(sql.c_str())));
		}
		CATCH_PASS_SQLITE_EXCEPTIONS(sql)
	}

	std::size_t Execute(const char* sql)
	{
		TRY 
		{
			SCOPED_LOG(m_Log);

			LOG_TRACE("Sql: [%s]") % sql;

			return m_DataBase.execDML(sql);
		}
		CATCH_PASS_SQLITE_EXCEPTIONS(sql)
	}

	void StartTransaction()
	{
		Execute("begin transaction;");
	}

	void Commit()
	{
		Execute("commit transaction;");
	}

	void RollBack()
	{
		Execute("rollback transaction;");
	}

	void Close()
	{
		TRY 
		{
			SCOPED_LOG(m_Log);

			m_DataBase.close();
		}
		CATCH_PASS_SQLITE_EXCEPTIONS("Close failed.")
	}	

	void Open(const char* szFile)
	{
		TRY 
		{
			SCOPED_LOG(m_Log);

			LOG_TRACE("DB file: [%s]") % szFile;

			m_DataBase.open(szFile);		
		}
		CATCH_PASS_SQLITE_EXCEPTIONS(szFile)
	}

	Recordset::Ptr Fetch(const std::string& sql)
	{
		TRY 
		{
			SCOPED_LOG(m_Log);

			return Recordset::Ptr(new Recordset(m_DataBase.execQuery(sql.c_str())));
		}
		CATCH_PASS_SQLITE_EXCEPTIONS(sql)
	}

	std::size_t ExecuteScalar(const std::string& sql)
	{
		TRY 
		{
			SCOPED_LOG(m_Log);

			return m_DataBase.execScalar(sql.c_str());
		}
		CATCH_PASS_SQLITE_EXCEPTIONS(sql)
	}

	__int64 LastRowId()
	{
		return m_DataBase.lastRowId();
	}

private:

	//! Database
	CppSQLite3DB		m_DataBase;

	//! Logger
	ILog&				m_Log;
};


SQLiteDataBase::SQLiteDataBase(ILog& logger)
	: m_pImpl(new Impl(logger))
{
}

SQLiteDataBase::~SQLiteDataBase(void)
{
	delete m_pImpl;
}

void SQLiteDataBase::Open(const std::string& arg)
{
	m_pImpl->Open(arg.c_str());
}

void SQLiteDataBase::Close()
{
	m_pImpl->Close();
}

void SQLiteDataBase::BeginTransaction()
{
	m_pImpl->StartTransaction();
}

void SQLiteDataBase::Commit()
{
	m_pImpl->Commit();
}

void SQLiteDataBase::Rollback()
{
	m_pImpl->RollBack();
}

IStatement::Ptr SQLiteDataBase::CreateStatement(const std::string& sql)
{
	return m_pImpl->CreateStatement(sql);
}

std::size_t SQLiteDataBase::Execute(const std::string& sql)
{
	return m_pImpl->Execute(sql.c_str());
}


Recordset::Ptr SQLiteDataBase::Fetch(const std::string& sql)
{
	return m_pImpl->Fetch(sql);
}


std::size_t SQLiteDataBase::ExecuteScalar(const std::string& sql)
{
	return m_pImpl->ExecuteScalar(sql);
}

__int64 SQLiteDataBase::LastRowId()
{
	return m_pImpl->LastRowId();
}

}


