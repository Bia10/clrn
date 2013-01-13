#include "stdafx.h"

#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>

#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

class ISerializable
{
public:
	typedef boost::shared_ptr<ISerializable> Ptr;
	virtual void Serialize(std::ostream& stream) = 0;
	virtual void Deserialize(std::istream& stream) = 0;
};

template<typename T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& data) 
{
 	BOOST_FOREACH(const T& value, data)
 		os << value;
	return os;
}

template<typename T>
std::istream& operator >> (std::istream& os, std::vector<T>& data) 
{
 	BOOST_FOREACH(T& value, data)
 		os >> value;
	return os;
}

template<typename T>
class Field : public ISerializable, boost::noncopyable
{
public:
	Field(T& field, const char* name, std::size_t version = 1)
		: m_Field(field)
		, m_Name(name)
		, m_Version(version) 
	{}

private:
	void Serialize(std::ostream& stream) override
	{
		stream << m_Name << " = '" << m_Field << "' ";
	}

	void Deserialize(std::istream& stream) override
	{
		stream >> m_Field;
	}

private:
	T& m_Field;
	std::string m_Name;
	std::size_t m_Version;
};

class Serializable : public ISerializable
{
	typedef std::vector<ISerializable::Ptr> Fields;
public:
	void Serialize(std::ostream& stream) override
	{
		if (m_Fields.empty())
			Register();

		BOOST_FOREACH(const ISerializable::Ptr& field, m_Fields)
			field->Serialize(stream);
	}

	void Deserialize(std::istream& stream) override
	{
		if (m_Fields.empty())
			Register();

		BOOST_FOREACH(const ISerializable::Ptr& field, m_Fields)
			field->Deserialize(stream);
	}

protected:

	template<typename T>
	void RegisterField(T& field, const char* name, std::size_t version = 1)
	{
		m_Fields.push_back(ISerializable::Ptr(new Field<T>(field, name, version)));
	}

private:

	virtual void Register() = 0;

private:

	Fields m_Fields;
};


#define SUSPENDABLE_BEGIN \
	virtual void Register() override {

#define SUSPENDABLE_END }

#define FIELD(name) RegisterField(name, #name);
#define NAMED_FIELD(field, name) RegisterField(field, name);
#define FIELD_FROM_VER(field, name, version) RegisterField(field, name, version);

class Test : public Serializable
{
	SUSPENDABLE_BEGIN
		FIELD(m_Int)
		NAMED_FIELD(m_String, "some_string")
		FIELD_FROM_VER(m_Strings, "strings", 2)
	SUSPENDABLE_END

public:
	Test()
	{

	}

//private:
	int m_Int;
	std::string m_String;
	std::vector<std::string> m_Strings;
};

// template< class A1, class M, class T >
// void bind( M T::*f, A1 a1 )
// {
// 
// }

void BytesToString(std::ostream& stream, const void* data, std::size_t size)
{
	for (std::size_t i = 0 ; i < size; ++i)
	{
		const unsigned char symbol = reinterpret_cast<const unsigned char*>(data)[i];
		stream 
			<< std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(symbol)
			<< std::setw(0) << "'" << std::dec;
		
		if (symbol)
			stream << symbol;
		else
			stream << "0";
		stream << "' ";
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		{
			std::ofstream ofs("D:\\1.txt");

			ofs << "test";
			ofs.flush();
			
		}

		std::ostringstream oss;

		std::vector<char> bytes;
		bytes.push_back('\0');
		bytes.push_back(255);
		bytes.push_back('A');
		bytes.push_back('v');
		bytes.push_back(253);
		bytes.push_back(252);

		BytesToString(oss, &bytes.front(), bytes.size());

		const std::string result = oss.str();


// 		Test t;
// 		t.m_Int = 100;
// 		t.m_String = "100";
// 		t.m_Strings.push_back("300");
// 
// 		std::stringstream oss;
// 
// 		t.Serialize(oss);
// 
// 		Test s;
// 		s.Deserialize(oss);

// 		boost::bind(&Test::m_Strings, &t);
// 		bind(&Test::m_Strings, &t);

	}
	catch (const std::exception& e)
	{	
		std::cout << e.what() << std::endl;
	}
	catch(...)
	{

	}

	return 0;
}

