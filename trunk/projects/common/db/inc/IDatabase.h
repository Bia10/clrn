#ifndef IDatabase_h__
#define IDatabase_h__

#include "IStatement.h"
#include "Recordset.h"

#include <string>

#include <boost/shared_ptr.hpp>

namespace sql
{

//! DataBase abstraction
class IDatabase
{
public:
	typedef boost::shared_ptr<IDatabase> Ptr;

	virtual ~IDatabase() {}
	virtual void Open(const std::string& arg) = 0;
	virtual void Close() = 0;
	virtual void BeginTransaction() = 0;
	virtual void Commit() = 0;
	virtual void Rollback() = 0;
	virtual std::size_t Execute(const std::string& sql) = 0;
	virtual std::size_t ExecuteScalar(const std::string& sql) = 0;
	virtual Recordset::Ptr Fetch(const std::string& sql) = 0;
	virtual IStatement::Ptr CreateStatement(const std::string& sql) = 0;

};

}

#endif // IDatabase_h__
