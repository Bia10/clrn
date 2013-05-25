#include "PacketParser.h"

#include <vector>
#include <sstream>

#include <boost/regex.hpp>

namespace clnt
{
namespace ps
{

class PacketParser::Impl
{
public:
	Impl(const std::string& regexp, const std::string& packet)
		: m_Regexp(regexp)
		, m_Data(packet)
		, m_It(m_Data.begin(), m_Data.end(), m_Regexp)
	{

	}

	bool Eof() const
	{
		return m_It == m_End;
	}

	void operator ++ ()
	{
		++m_It;
	}

	unsigned GetInt(unsigned index)
	{
		const std::string data = (*m_It)[index];

		std::vector<char> result;
		result.reserve(data.size() / 5);

		std::string::const_iterator it = data.begin();
		while (*it == ' ')
			++it;

		const std::string::const_iterator itEnd = data.end();
		for (; it != itEnd; ++it)
		{
			const std::string byteAsString(it, it + 2);

			int value = 0;

			std::stringstream ss;
			ss << std::hex << byteAsString;
			ss >> value;
			result.push_back(static_cast<char>(value));

			it += 5; // remaining char plus char in quotes and space

			if (it == itEnd)
				break;
		}

		assert(result.size() <= sizeof(int));
		while (result.size() != sizeof(int))
			result.insert(result.begin(), 0);

		std::reverse(result.begin(), result.end());
		return *reinterpret_cast<const unsigned*>(&result.front());
	}

	std::string GetString(unsigned index)
	{
		const std::string data = (*m_It)[index];

		if (data.size() < 2)
			return data;

		std::string result;
		result.reserve(data.size() / 5);

		std::string::const_iterator it = data.begin();
		while (*it == ' ')
			++it;

		const std::string::const_iterator itEnd = data.end();
		for (; it != itEnd; ++it)
		{
			const std::string byteAsString(it, it + 2);

			int value = 0;

			std::stringstream ss;
			ss << std::hex << byteAsString;
			ss >> value;
			result.push_back(static_cast<char>(value));

			it += 5; // remaining char plus char in quotes and space

			if (it == itEnd)
				break;
		}

		return result;
	}

private:

	boost::regex m_Regexp;
	const std::string m_Data;
	boost::sregex_iterator m_It;
	boost::sregex_iterator m_End;
};

PacketParser::PacketParser(const std::string& regexp, const std::string& packet)
	: m_Impl(new Impl(regexp, packet))
{

}

bool PacketParser::Eof() const
{
	return m_Impl->Eof();
}

PacketParser& PacketParser::operator++()
{
	m_Impl->operator++();
	return *this;
}

unsigned PacketParser::GetInt(unsigned index)
{
	return m_Impl->GetInt(index);
}

std::string PacketParser::GetString(unsigned index)
{
	return m_Impl->GetString(index);
}

PacketParser::~PacketParser()
{
	delete m_Impl;
}

}
}
