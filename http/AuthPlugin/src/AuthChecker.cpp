/*
 * AuthChecker.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: root
 */

#include "AuthChecker.h"
#include "Singletones.h"
#include "Converter.h"
#include "UserInfo.h"

#include <vector>
#include <stdexcept>
#include <fstream>

#include <boost/date_time/posix_time/posix_time.hpp>


CAuthChecker::CAuthChecker()
{

}

CAuthChecker::~CAuthChecker()
{

}

const TCHAR *CAuthChecker::Description()
{
	return _T("Authorization checker.");
}



const bool CAuthChecker::Match(const http::IHttpData & Info)
{
	// Check hash
	const _tstring& sAuthHash = Info.GetCookieValue(_T("session_hash"));
	if (sAuthHash.empty() || !IsHashValid(sAuthHash))
		return true;

	return false;
}



void CAuthChecker::Execute(const http::IHttpData & Info, http::IHttpData & Reply)
{
	// Remember page which user requested
	const _tstring& sRequestedPath = Info.GetRequestPath();

	// Set it up as cookie
	const _tstring sLastRequestedPath = Info.GetCookieValue(_T("requested_path"));
	if (sLastRequestedPath.empty())
	{
		Reply.SetCookieValue(_T("requested_path"), sRequestedPath);
	}

	boost::posix_time::ptime ptTime = boost::posix_time::second_clock::local_time() + boost::posix_time::seconds(60);

	Reply.SetCookieValue(_T("expires"), cmn::CConverter::ToCookieExpireTime(ptTime));

	// This method invokes if auth check in method Match() failed,
	// so we need to redirect user to authorization page.
	_tstring sPath = Settings::Instance().GetDocRoot();
	sPath.append(_T("login.html"));

	Reply.AppendFileName(sPath);
}

const bool CAuthChecker::IsHashValid(const _tstring & sHash)
{
	try
	{
		// Check if user with this hash exists
		Users::Instance().Get(sHash);
		return true;
	}
	catch(std::exception&)
	{
		Logger::Instance().Write(__FILE__, __LINE__, _T("User not found."), cmn::ILog::Level::eDebug);
	}

	return false;
}





