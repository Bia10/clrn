/*
 * Request.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: root
 */

#include "DefaultRequestExecutor.h"
#include "Exception.h"
#include "ServerLogger.h"
#include "ServerSettings.h"
#include "Status.h"
#include "MimeType.h"
#include "HttpData.h"
#include "CacheManager.h"

#include <fstream>

#include <boost/algorithm/string/compare.hpp>

namespace srv
{

CDefaultRequestExecutor::CDefaultRequestExecutor()
{
	SCOPED_LOG_FUNCTION
	m_sDocumentsRoot = cmn::Settings::Instance().GetDocRoot();
}

CDefaultRequestExecutor::~CDefaultRequestExecutor()
{
	SCOPED_LOG_FUNCTION
}

void CDefaultRequestExecutor::Execute(const http::IHttpData& RequestInfo, http::IHttpData& Reply)
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		m_pRequest = &RequestInfo;

		/*if (GetCachedReply(Reply))
			return;*/

		ProcessRequest(RequestInfo, Reply);

		/*const _tstring& sMethod = m_pRequest->GetRequestMethod();
		if (sMethod == _T("GET"))
		{
			const http::CHttpData * pRequest = reinterpret_cast< const http::CHttpData * > (&RequestInfo);
			http::CHttpData * pReply = reinterpret_cast< http::CHttpData * > (&Reply);
			CacheManager::Instance().Add(*pRequest, *pReply, false);
		}*/
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

const bool CDefaultRequestExecutor::GetCachedReply(http::IHttpData& Reply) const
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		assert(m_pRequest);

		const http::CHttpData& pRequest = reinterpret_cast< const http::CHttpData& > (*m_pRequest);
		http::CHttpData& pReply = reinterpret_cast< http::CHttpData& > (Reply);
		return CacheManager::Instance().Get(pRequest, pReply);
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());

	return false;
}

const TCHAR *CDefaultRequestExecutor::Description()
{
	return _T("Default request executor.");
}

const bool CDefaultRequestExecutor::Match(const http::IHttpData & RequestInfo)
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{

	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());

	return true;
}

void CDefaultRequestExecutor::ProcessRequest(const http::IHttpData& RequestInfo, http::IHttpData& Reply)
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		m_sPath = RequestInfo.GetRequestPath();

		_tstring sExtension;

		// If path is a directory then add "index.html".
		if (m_sPath[m_sPath.size() - 1] == '/')
		{
			m_sPath.append("index.html");
			sExtension = _T("html");
		}
		else
		{
			sExtension = RequestInfo.GetRequestExtension();
		}

		const _tstring sAccept = RequestInfo.GetHeaderValue(_T("Accept"));
		if (sAccept != _T("*/*") && sAccept.find(sExtension) == _tstring::npos)
		{
			sExtension = _T("html");
		}

		// Full path
		_tstring sFullPath = m_sDocumentsRoot;
		sFullPath.append(m_sPath);

		Reply.AppendFileName(sFullPath);

		http::CMimeType MimeType(sExtension);
		Reply.SetMimeType(MimeType.ToString().c_str());
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

} /* namespace srv */
