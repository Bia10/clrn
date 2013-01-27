#ifndef Recordset_h__
#define Recordset_h__

#include "Modules.h"
#include "CppSQLite3.h"

#include <vector>

#include <boost/shared_ptr.hpp>

namespace sql
{
const int CURRENT_MODULE_ID = Modules::DataBase;

//! Record set implementation
class Recordset
{
public:
	typedef boost::shared_ptr<Recordset> Ptr;
	struct Data
	{
		Data(const unsigned char* data, int length) : m_Data(data), m_Length(length) {}
		const unsigned char* m_Data;
		const int m_Length;
	};

	Recordset(const CppSQLite3Query& query) : m_Query(query) {}

	Recordset& operator ++ (int)
	{
		m_Query.nextRow();
		return *this;
	}

	Recordset& operator ++ ()
	{
		m_Query.nextRow();
		return *this;
	}

	bool Eof() const
	{
		return m_Query.eof();
	}

	template<typename T>
	const T Get(int field) const;

	template<typename T>
	const T Get(const std::string& field) const;

	template<>
	const int Get(int field) const
	{
		return m_Query.getIntField(field);
	}

	template<>
	const int Get(const std::string& field) const
	{
		return m_Query.getIntField(field.c_str());
	}

	template<>
	const double Get(int field) const
	{
		return m_Query.getFloatField(field);
	}

	template<>
	const double Get(const std::string& field) const
	{
		return m_Query.getFloatField(field.c_str());
	}

	template<>
	const std::string Get(int field) const
	{
		return m_Query.getStringField(field);
	}

	template<>
	const std::string Get(const std::string& field) const
	{
		return m_Query.getStringField(field.c_str());
	}

	template<>
	const Data Get(int field) const
	{
		int length = 0;
		const unsigned char* ptr = m_Query.getBlobField(field, length);
		return Data(ptr, length);
	}

	template<>
	const Data Get(const std::string& field) const
	{
		int length = 0;
		const unsigned char* ptr = m_Query.getBlobField(field.c_str(), length);
		return Data(ptr, length);
	}

	template<>
	const char* const Get(int field) const
	{
		return m_Query.fieldValue(field);
	}

	template<>
	const char* const Get(const std::string& field) const
	{
		return m_Query.fieldValue(field.c_str());
	}

private:
	mutable CppSQLite3Query m_Query;
};
}
#endif // Recordset_h__
