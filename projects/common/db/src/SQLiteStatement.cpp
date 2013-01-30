#include "SQLiteStatement.h"

namespace sql
{

void SQLiteStatement::Execute()
{
	m_Stmnt.execDML();
	m_Stmnt.reset();
	m_Index = 1;
}

IStatement& SQLiteStatement::operator<<(const std::string& value)
{
	m_Stmnt.bind(m_Index++, value.c_str());
	return *this;
}

IStatement& SQLiteStatement::operator<<(const char* value)
{
	m_Stmnt.bind(m_Index++, value);
	return *this;
}

IStatement& SQLiteStatement::operator<<(const double value)
{
	m_Stmnt.bind(m_Index++, value);
	return *this;
}

IStatement& SQLiteStatement::operator<<(const int value)
{
	m_Stmnt.bind(m_Index++, value);
	return *this;
}

IStatement& SQLiteStatement::operator<<(const Recordset::Data& value)
{
	m_Stmnt.bind(m_Index++, value.m_Data, value.m_Length);
	return *this;
}

IStatement& SQLiteStatement::operator>>(std::string& value)
{
	m_Stmnt.bind(m_Index++, value.c_str());
	return *this;
}

IStatement& SQLiteStatement::operator>>(char* value)
{
	m_Stmnt.bind(m_Index++, value);
	return *this;
}

IStatement& SQLiteStatement::operator>>(double& value)
{
	m_Stmnt.bind(m_Index++, value);
	return *this;
}

IStatement& SQLiteStatement::operator>>(int& value)
{
	m_Stmnt.bind(m_Index++, value);
	return *this;
}

IStatement& SQLiteStatement::operator>>(Recordset::Data& value)
{
	m_Stmnt.bind(m_Index++, value.m_Data, value.m_Length);
	return *this;
}

IStatement& SQLiteStatement::operator>>(unsigned int& value)
{
	m_Stmnt.bind(m_Index++, reinterpret_cast<int&>(value));
	return *this;
}

IStatement& SQLiteStatement::operator>>(Null)
{
	m_Stmnt.bindNull(m_Index++);
	return *this;
}

IStatement& SQLiteStatement::operator<<(const unsigned int value)
{
	m_Stmnt.bind(m_Index++, static_cast<int>(value));
	return *this;
}

IStatement& SQLiteStatement::operator<<(Null)
{
	m_Stmnt.bindNull(m_Index++);
	return *this;
}

}