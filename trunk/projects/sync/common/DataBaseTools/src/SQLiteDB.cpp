#include "SQLiteDB.h"
#include "CppSQLite3.h"
#include "Exception.h"
#include "Constants.h"
#include "data.pb.h"
#include "StreamHelpers.h"
#include "Table.h"

#include <sstream>

#include <boost/thread/mutex.hpp>

static const unsigned int CURRENT_MODULE_ID =  DATABASE_MODULE_ID;

#define CATCH_PASS_SQLITE_EXCEPTION(message)										\
catch(CppSQLite3Exception& e)														\
{																					\
	std::ostringstream oss;															\
	oss << "SQLite error: [" << e.errorCode() << "] Message: ["						\
	<< e.errorMessage() << "] Sql: [" << message << "]";							\
	throw CMN_NAMESPACE_NAME::CExcept(__FILE__, __LINE__, oss.str());				\
}

#define CATCH_PASS_SQLITE_EXCEPTIONS(message)										\
	CATCH_PASS_SQLITE_EXCEPTION(message)											\
	CATCH_PASS_EXCEPTIONS(message)					

//! SQLite database wrapper implementation
//!
//! \class CSQLiteDB
//! 
class DataBase::Impl
{
public:

	Impl(ILog& logger, const char* szFile)
		: m_Log(logger)
	{
		Open(szFile);
	}	

	~Impl()
	{
		Close();
	}

	void Execute(const char* sql, data::Table& result, const data::Table_Id tableId)
	{
		TRY 
		{
			SCOPED_LOG(m_Log);

			boost::mutex::scoped_lock lock(m_Mutex);
			CppSQLite3Query query = m_DataBase.execQuery(sql);
			
			CTableMapping::CColumnMapping::ColumnMap mapping;

			for (int field = 0 ; field < query.numFields(); ++field)
			{
				const char* name = query.fieldName(field);
				const int type = query.fieldDataType(field);

				mapping.insert(std::make_pair(name, field));

				LOG_DEBUG("Column index: [%s], name: [%s], type: [%s]") % field % name % type;
			}

			if (data::Table_Id_None == tableId)
				result.set_id(CTableMapping::Instance()[mapping]);
			else
				result.set_id(tableId);

			while (!query.eof())
			{
				data::Table_Row* row = result.add_rows();

				for (int field = 0 ; field < query.numFields(); ++field)
				{
					const char* value = query.fieldValue(field);
					row->add_data(value ? value : "NULL");					
				}
				query.nextRow();	
			}

			LOG_TRACE("Sql: [%s], rows received: [%s]") % sql % result.rows_size();
			LOG_DEBUG("Table: [%s]") % result.DebugString();
		}
		CATCH_PASS_SQLITE_EXCEPTIONS(sql)
	}

	std::size_t Execute(const char* sql)
	{
		TRY 
		{
			SCOPED_LOG(m_Log);

			LOG_TRACE("Sql: [%s]") % sql;

			boost::mutex::scoped_lock lock(m_Mutex);
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

private:

	void Open(const char* szFile)
	{
		TRY 
		{
			SCOPED_LOG(m_Log);

			LOG_TRACE("DB file: [%s]") % szFile;

			boost::mutex::scoped_lock lock(m_Mutex);
			m_DataBase.open(szFile);		
		}
		CATCH_PASS_SQLITE_EXCEPTIONS(szFile)
	}

	void Close()
	{
		TRY 
		{
			SCOPED_LOG(m_Log);

			boost::mutex::scoped_lock lock(m_Mutex);
			m_DataBase.close();
		}
		CATCH_PASS_SQLITE_EXCEPTIONS("Close failed.")
	}	

	//! Database
	CppSQLite3DB		m_DataBase;

	//! Logger
	ILog&				m_Log;

	//! DB mutex
	boost::mutex		m_Mutex;
};


std::auto_ptr<DataBase> DataBase::s_Instance;

DataBase::DataBase(ILog& logger, const char* szFile)
	: m_pImpl(new Impl(logger, szFile))
{
}

DataBase::~DataBase(void)
{
	delete m_pImpl;
}

void DataBase::Execute(const char* sql, data::Table& result, const data::Table_Id tableId /*= static_cast<data::Table_Id>(0)*/)
{
	m_pImpl->Execute(sql, result, tableId);
}

unsigned int DataBase::Execute(const char* sql)
{
	return m_pImpl->Execute(sql);
}

void DataBase::StartTransaction()
{
	m_pImpl->StartTransaction();
}

void DataBase::Commit()
{
	m_pImpl->Commit();
}

void DataBase::RollBack()
{
	m_pImpl->RollBack();
}

void DataBase::Create(ILog& logger, const char* szFile)
{
	s_Instance.reset(new DataBase(logger, szFile));
}

void DataBase::Shutdown()
{
	s_Instance.reset();
}

DataBase& DataBase::Instance()
{
	return *s_Instance;
}
