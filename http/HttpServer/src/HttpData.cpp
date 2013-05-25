/*
 * HttpData.cpp
 *
 *  Created on: Nov 13, 2011
 *      Author: root
 */

#include "HttpData.h"
#include "ServerLogger.h"
#include "Converter.h"
#include "MimeType.h"
#include "Exception.h"

#include <algorithm>
#include <fstream>

#include <boost/iterator.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>


namespace http
{

static const _tstring gs_sEmptyString;

CHttpData::CHttpData() :
		m_nDataIndex(0),
		m_sStatus(_T("HTTP/1.0 200 OK"))
{
	SCOPED_LOG_FUNCTION
}

CHttpData::~CHttpData()
{
	SCOPED_LOG_FUNCTION
}

const _tstring&	CHttpData::GetHeaderValue(const _tstring& sName) const
{
	THeaders::const_iterator it = m_mapHeaders.find(sName);
	if (m_mapHeaders.end() == it)
	{
		return gs_sEmptyString;
	}
	return it->second;
}

const TCHAR *CHttpData::GetPostData() const
{
	return &m_vecBuffer[m_nDataIndex];
}

void CHttpData::SetHeader(const _tstring& sName, const _tstring& sValue)
{
	m_mapHeaders[sName] = sValue;
}

void CHttpData::AppendContent(const _tstring & sContent)
{
	SCOPED_LOG_FUNCTION

	AppendContent(sContent.c_str(), sContent.size());
}

void CHttpData::AppendContent(const TCHAR *szContent, const std::size_t & nLength)
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		m_vecBuffer.reserve(m_vecBuffer.size() + nLength);
		std::copy(szContent, szContent + nLength, std::back_inserter(m_vecBuffer));
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

void CHttpData::AppendFileName (const _tstring& sFileName)
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		// Open the file
		_tifstream isContent(sFileName.c_str(), std::ios::in | std::ios::binary);
		if (!isContent)
		{
			_tostringstream ossError;
			ossError << "Requested path " << sFileName << " not found." << std::endl;
			THROW_EXCEPTION_CODE(cmn::CStatus::StatusCode::eNotFound, ossError.str());
		}

		boost::filesystem::path pthFile(sFileName);

		const std::size_t nFileSize = boost::filesystem::file_size(pthFile);

		const std::size_t nBeginIndex = m_vecBuffer.size();
		m_vecBuffer.resize(m_vecBuffer.size() + nFileSize);
		isContent.read(&m_vecBuffer[nBeginIndex], nFileSize);

		_tstring sExtension = pthFile.extension().string();
		if (sExtension.empty())
		{
			sExtension = _T("html");
		}
		else
		{
			sExtension.assign(&sExtension.c_str()[1]);
		}

		http::CMimeType MimeType(sExtension);
		SetMimeType(MimeType.ToString());
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

void CHttpData::SetMimeType(const _tstring&	sMimeType)
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		m_mapHeaders[_T("Content-Type")] = sMimeType;
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

void CHttpData::SetStatus(const _tstring& sStatus)
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		m_sStatus.assign(sStatus);
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

IHttpData::TAsioBuffers CHttpData::GetReplyBuffers()
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		// Checking type, status
		if (!m_mapHeaders.count(_T("Content-Type")))
			m_mapHeaders[_T("Content-Type")] = _T("text/html");

		if (m_sStatus.empty())
			THROW_EXCEPTION(_T("Reply status is empty."));

		// Setting up content length and software
		m_mapHeaders[_T("Content-Length")] = boost::lexical_cast< _tstring >(m_vecBuffer.size());
		m_mapHeaders[_T("Server")] = _T("CLRN_http_server");

		// Filling result vector
		TAsioBuffers vecBuffers;

		static const _tstring s_sNameValueSeparator(_T(": "));
		static const _tstring s_sCRLF(_T("\r\n"));

		// First string contains reply status
		vecBuffers.push_back(boost::asio::buffer(m_sStatus));
		vecBuffers.push_back(boost::asio::buffer(s_sCRLF));

		// Adding all headers
		for (const auto& Header : m_mapHeaders)
		{
			vecBuffers.push_back(boost::asio::buffer(Header.first));
			vecBuffers.push_back(boost::asio::buffer(s_sNameValueSeparator));
			vecBuffers.push_back(boost::asio::buffer(Header.second));
			vecBuffers.push_back(boost::asio::buffer(s_sCRLF));
		}

		vecBuffers.push_back(boost::asio::buffer(s_sCRLF));
		vecBuffers.push_back(boost::asio::buffer(m_vecBuffer));

		return vecBuffers;
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

IHttpData::TBuffer & CHttpData::GetRawBuffer()
{
	return m_vecBuffer;
}

const _tstring&	CHttpData::GetRequestMethod() const
{
	return m_sMethod;
}

const _tstring&	CHttpData::GetRequestPath() const
{
	return m_sPath;
}

const _tstring&	CHttpData::GetRequestExtension() const
{
	return m_sExtension;
}

const _tstring CHttpData::GetCookieValue(const _tstring& sName) const
{
	THeaders::const_iterator it = m_mapHeaders.find(_T("Cookie"));
	if (m_mapHeaders.end() == it)
	{
		return gs_sEmptyString;
	}

	const _tstring& sCookies = it->second;

	// Getting posistion of cookie name
	const _tstring::size_type nCookieName = sCookies.find(sName + '=');
	if (_tstring::npos == nCookieName)
	{
		return gs_sEmptyString;
	}

	const _tstring::size_type nValueBegin = nCookieName + sName.size() + 1;

	// Gettings position of the end of cookie value
	const _tstring::size_type nValueEnd = sCookies.find(';', nValueBegin);
	if (_tstring::npos != nValueEnd)
	{
		return sCookies.substr(nValueBegin, nValueEnd - nValueBegin);
	}

	return &sCookies.c_str()[nValueBegin];
}

void CHttpData::SetCookieValue(const _tstring& sName, const _tstring& sValue)
{
	_tstring sSettedCookie;
	if (m_mapHeaders.count(_T("Set-Cookie")))
	{
		sSettedCookie = m_mapHeaders[_T("Set-Cookie")];
	}

	// Getting posistion of cookie name
	const _tstring::size_type nCookieName = sSettedCookie.find(sName + '=');
	if (_tstring::npos != nCookieName)
	{
		const _tstring::size_type nValueBegin = nCookieName + sName.size() + 1;

		// Gettings position of the end of cookie value
		const _tstring::size_type nValueEnd = sSettedCookie.find(';', nValueBegin);
		if (_tstring::npos != nValueEnd)
		{
			sSettedCookie.erase(nValueBegin, nValueEnd - nValueBegin);
		}
		else
		{
			sSettedCookie.erase(nValueBegin, sSettedCookie.size() - nValueBegin);
		}
	}

	// If value is empty just erasing cookie
	if (!sValue.empty())
	{
		sSettedCookie.append(sName + _T("=") + sValue + _T("; "));
	}

	m_mapHeaders[_T("Set-Cookie")] = sSettedCookie;
}

const CHttpData::TRequestParams& CHttpData::GetRequestParams() const
{
	return m_mapParams;
}

void CHttpData::Parse()
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		// Parse first line containing method, path, protocol
		static boost::basic_regex< TCHAR > s_rgxMethod(_T("(\\S+)\\s(\\S+)\\s(\\S+)"));

		// First line end
		const TBuffer::const_iterator itEndLine = std::find(m_vecBuffer.begin(), m_vecBuffer.end(), '\r');

		boost::match_results< TBuffer::const_iterator > Results;
		if (!boost::regex_match< TBuffer::const_iterator >(m_vecBuffer.begin(), itEndLine, Results, s_rgxMethod))
		{
			THROW_EXCEPTION_CODE(cmn::CStatus::StatusCode::eBadRequest, "Failed to parse first line of the http request.");
		}

		m_sMethod = Results[1].str();
		m_sPath = Results[2].str();

		if (_tstring::npos != m_sPath.find('%'))
		{
			_tostringstream ossResultUrl;
			cmn::CConverter::DecodeUrl(m_sPath, ossResultUrl);
			m_sPath = ossResultUrl.str();
		}

		if (m_sPath.empty())
		{
			THROW_EXCEPTION("Request path is empty.");
		}

		// Request path must be absolute and not contain "..".
		if (m_sPath[0] != '/' || m_sPath.find("..") != std::string::npos)
		{
			THROW_EXCEPTION_CODE(cmn::CStatus::StatusCode::eBadRequest, "Invalid url string.");
		}

		const _tstring::size_type nQuestion = m_sPath.rfind('?');
		if (_tstring::npos != nQuestion)
		{
			m_sPath.resize(nQuestion);
			ParseParams(&m_sPath.c_str()[nQuestion + 1]);
		}

		  // Determine the file extension.
		std::size_t nLastSlash = m_sPath.find_last_of("/");
		std::size_t nLastDot = m_sPath.find_last_of(".");
		if (nLastDot != std::string::npos && nLastDot > nLastSlash)
		{
			m_sExtension = &m_sPath.c_str()[nLastDot + 1];
		}

		// Find CRLF
		TBuffer::const_iterator itCRLF = std::find(m_vecBuffer.begin(), m_vecBuffer.end(), '\r');
		while (itCRLF != m_vecBuffer.end() && *(itCRLF + 2) != '\r')
			itCRLF = std::find< TBuffer::const_iterator >(itCRLF + 1, m_vecBuffer.end(), '\r');

		std::advance(itCRLF, 4);

		// Data after CRLF
		m_nDataIndex = std::distance< TBuffer::const_iterator >(m_vecBuffer.begin(), itCRLF);

		// Correcting buffer size
		m_vecBuffer.resize(m_nDataIndex + std::strlen(&*itCRLF));

		// Parse headers
		static boost::basic_regex< TCHAR > s_rgxHeaders(_T("([[:word:]-]+):\\s(.*?)\\r"));

		boost::regex_iterator< TBuffer::const_iterator > it(m_vecBuffer.begin(), m_vecBuffer.end(), s_rgxHeaders);
		const boost::regex_iterator< TBuffer::const_iterator > itEnd;

		std::for_each(it, itEnd, [&](const boost::match_results< TBuffer::const_iterator >& Match){

			// Inserting parsed field
			m_mapHeaders.insert(std::make_pair(Match[1].str(), Match[2].str()));
		});

	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

void CHttpData::ParseParams(const _tstring& sParams)
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		typedef boost::iterator_range< _tstring::const_iterator > TString;

		std::vector< TString > vecParams;

		boost::algorithm::split(vecParams, sParams, [](const TCHAR& cSymbol){return cSymbol == '&';});

		for (const TString& Param : vecParams)
		{
			const _tstring::const_iterator itEqual = std::find(Param.begin(), Param.end(), '=');

			if (Param.end() == itEqual)
			{
				THROW_EXCEPTION_CODE(cmn::CStatus::StatusCode::eBadRequest, "Bad request parameters format.");
			}

			m_mapParams.insert(std::make_pair(
						_tstring(Param.begin(), itEqual),
						_tstring(itEqual + 1, Param.end())
					)
			);
		}
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

} /* namespace http */
