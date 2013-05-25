/*
 * Exception.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: root
 */

#include "Exception.h"

//! Include project headers
#include "Types.h"

#include <boost/format.hpp>

namespace cmn
{


CException::CException(const char *szFile, const std::size_t & nLine, const CStatus::StatusCode & ecCode, const char *szMessage) :
	  m_sMessage(Format(szFile, nLine, ecCode, szMessage)),
	  m_ecError(ecCode)
{

}

CException::CException(const char *szFile, const std::size_t & nLine, const CStatus::StatusCode & ecCode, const wchar_t *szMessage) :
	m_sMessage(Format(szFile, nLine, ecCode, szMessage)),
	m_ecError(ecCode)
{

}

CException::CException(const char *szFile, const std::size_t & nLine, const CStatus::StatusCode & ecCode, const std::string& sMessage) :
	m_sMessage(Format(szFile, nLine, ecCode, sMessage.c_str())),
	m_ecError(ecCode)
{

}

CException::CException(const char *szFile, const std::size_t & nLine, const CStatus::StatusCode & ecCode, const std::wstring& sMessage) :
	m_sMessage(Format(szFile, nLine, ecCode, sMessage.c_str())),
	m_ecError(ecCode)
{

}

CException::~CException() throw()
{

}

const char *CException::what() const throw()
{
	return m_sMessage.c_str();
}

const CStatus::StatusCode& CException::code() const throw()
{
	return m_ecError;
}

const std::string CException::Format(const char *szFile, const std::size_t & nLine, const CStatus::StatusCode & ecCode, const char *szMessage) const
{
	return (boost::format("Text: {%s}\nCode: %s\nFile: %s\nLine: %s\n") % szMessage % CStatus::ToString(ecCode) % szFile % nLine).str();
}

const std::string CException::Format(const char *szFile, const std::size_t & nLine, const CStatus::StatusCode & ecCode, const wchar_t *szMessage) const
{
	return (boost::format("Text: {%s}\nCode: %s\nFile: %s\nLine: %s\n") % szMessage % CStatus::ToString(ecCode) % szFile % nLine).str();
}

} /* namespace cmn */
