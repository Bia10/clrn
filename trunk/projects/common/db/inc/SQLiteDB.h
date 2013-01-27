#ifndef SQLiteDB_h__
#define SQLiteDB_h__

#include "ILog.h"
#include "IDatabase.h"

#include <memory>

namespace sql
{

//! SQLite database wrapper
//!
//! \class CSQLiteDB
//! 
class SQLiteDataBase : public IDatabase
{
public:
	SQLiteDataBase(ILog& logger);
	~SQLiteDataBase();

	virtual void Open(const std::string& arg) override;
	virtual void Close() override;
	virtual void BeginTransaction() override;
	virtual void Commit() override;
	virtual void Rollback() override;
	virtual std::size_t Execute(const std::string& sql) override;
	virtual std::size_t ExecuteScalar(const std::string& sql) override;
	virtual Recordset::Ptr Fetch(const std::string& sql) override;
	virtual IStatement::Ptr CreateStatement(const std::string& sql) override;

private:

	//! Implementation
	class Impl;
	Impl*									m_pImpl;

	//! Instance
	static std::auto_ptr<SQLiteDataBase>	s_Instance;

};

}


#endif // SQLiteDB_h__
