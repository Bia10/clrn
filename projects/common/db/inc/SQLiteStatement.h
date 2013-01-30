#ifndef SQLiteStatement_h__
#define SQLiteStatement_h__

#include "CppSQLite3.h"

#include "IStatement.h"

namespace sql
{

class SQLiteStatement : public IStatement
{
public:
	SQLiteStatement(const CppSQLite3Statement& stmnt) : m_Stmnt(stmnt), m_Index(1) {}

	virtual void Execute() override;
	virtual IStatement& operator << (const std::string& value) override;
	virtual IStatement& operator << (const char* value) override;
	virtual IStatement& operator << (const double value) override;
	virtual IStatement& operator << (const int value) override;
	virtual IStatement& operator << (const unsigned int value) override;
	virtual IStatement& operator << (const Recordset::Data& value) override;
	virtual IStatement& operator << (Null) override;
	virtual IStatement& operator >> (std::string& value) override;
	virtual IStatement& operator >> (char* value) override;
	virtual IStatement& operator >> (double& value) override;
	virtual IStatement& operator >> (int& value) override;
	virtual IStatement& operator >> (unsigned int& value) override;
	virtual IStatement& operator >> (Recordset::Data& value) override;
	virtual IStatement& operator >> (Null) override;
private:
	CppSQLite3Statement m_Stmnt;
	int m_Index;
};

}

#endif // SQLiteStatement_h__
