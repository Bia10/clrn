#ifndef SQLiteDB_h__
#define SQLiteDB_h__

#include "ILog.h"

#include <memory>

//! Forward declarations
namespace data
{
	class Table;
	enum Table_Id;
}

//! SQLite database wrapper
//!
//! \class CSQLiteDB
//! 
class DataBase
{
public:
	DataBase(ILog& logger, const char* szFile);
	~DataBase(void);

	//! Execute query
	void				Execute(const char* sql, data::Table& result, const data::Table_Id tableId = static_cast<data::Table_Id>(0));

	//! Execute DML
	unsigned int		Execute(const char* sql);

	//! Begin transaction
	void				StartTransaction();

	//! Commit transaction
	void				Commit();

	//! Rollback transaction
	void				RollBack();

	//! Instance initialization
	static void			Create(ILog& logger, const char* szFile);

	//! Instance deinitialization
	static void			Shutdown();

	//! Instance reference
	static DataBase&	Instance();

private:

	//! Implementation
	class Impl;
	Impl*							m_pImpl;

	//! Instance
	static std::auto_ptr<DataBase>	s_Instance;

};
#endif // SQLiteDB_h__
