#include "stdafx.h"
#include "Exception.h"

//! Exception implementation
//!
//! \class Impl
//!
class CExcept::Impl
{
public:
	Impl(const char* szFile, const unsigned long line, const std::string& text)
		: m_IsMessageBuilded(false)
	{
		Format(szFile, line, text);
	}

	Impl(const Impl& that)
		: m_Buffer(that.m_Buffer.str())
		, m_IsMessageBuilded(false)
	{
	}

	~Impl()
	{

	}

	void Format(const char* szFile, const unsigned long line, const std::string& text)
	{
		m_IsMessageBuilded = false;

		m_Buffer 
			<< "File: [" << szFile 
			<< "], line: [" << line
			<< "], text: [" << text << "]." << std::endl;
	}

	const char* What() const throw()
	{
		if (!m_IsMessageBuilded)
		{
			try 
			{
				m_Message = m_Buffer.str();
				m_IsMessageBuilded = true;
			}
			catch(...)
			{

			}
		}
		return m_Message.c_str();
	}

	std::basic_ostream<char>& Append()
	{
		m_IsMessageBuilded = false;
		return m_Buffer;
	}

	std::basic_ostream<char>& Append(const char* szFile, const unsigned long line)
	{
		m_IsMessageBuilded = false;
		m_Buffer.seekp(0, std::ios::end);
		m_Buffer << "File: [" << szFile << "], line: [" << line << "]." << std::endl;
		m_Buffer.seekp(0, std::ios::end);
		return m_Buffer;
	}

private:
	
	//! Message
	std::ostringstream	m_Buffer;

	//! Builded
	mutable bool		m_IsMessageBuilded;

	//! Message
	mutable std::string	m_Message;
};

CExcept::CExcept(const char* szFile, const unsigned long line, const char* text)
	: m_pImpl(new Impl(szFile, line, text))
{

}

CExcept::CExcept(const char* szFile, const unsigned long line, const std::string& text)
	: m_pImpl(new Impl(szFile, line, text))
{

}


CExcept::CExcept(const CExcept& xcpt)
	: m_pImpl(new Impl(*xcpt.m_pImpl))
{

}

CExcept::~CExcept()
{
}

const char* CExcept::what() const throw()
{
	return m_pImpl->What();
}

std::basic_ostream<char>& CExcept::Append(const char* szFile, const unsigned long line)
{
	return m_pImpl->Append(szFile, line);
}

std::basic_ostream<char>& CExcept::Append()
{
	return m_pImpl->Append();
}

namespace xc_details
{

void FormatArguments(StringList& args, const char* text /*= 0*/)
{
	if (!text)
		return;

	boost::algorithm::split(args, text, boost::algorithm::is_any_of(","));

	StringList::iterator it = args.begin();
	const StringList::const_iterator itEnd = args.end();
	for (; it != itEnd; ++it)
	{
		if (it->size() > 1 && std::find(it->begin(), it->begin() + 2, '\"') != it->begin() + 2)
			it->clear();
		else
			boost::algorithm::trim(*it);
	}
}

} // namespace xc_details