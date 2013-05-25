/*
 * Authorizer.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: root
 */

#include "Authorizer.h"
#include "Singletones.h"
#include "Exception.h"
#include "UserInfo.h"
#include "Crypto.h"

#include <vector>
#include <fstream>

#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>


CAuthorizer::CAuthorizer()
{
	// TODO Auto-generated constructor stub

}

CAuthorizer::~CAuthorizer()
{
	// TODO Auto-generated destructor stub
}

const TCHAR *CAuthorizer::Description()
{
	return _T("Authorization executor");
}

const bool CAuthorizer::Match(const http::IHttpData & Info)
{
	// If params after '?' is empty action cannot be completed
	const http::IHttpData::TRequestParams& mapParams = Info.GetRequestParams();
	if (mapParams.empty())
	{
		return false;
	}

	// If request path is "/authorize" we should handle this request
	const _tstring& sPath = Info.GetRequestPath();
	if (sPath == _T("/authorize"))
	{
		return true;
	}
	return false;
}

void CAuthorizer::Execute(const http::IHttpData & Info, http::IHttpData & Reply)
{
	try
	{
		// Getting user login and password
		http::IHttpData::TRequestParams mapParams = Info.GetRequestParams();
		const _tstring& sLogin = mapParams[_T("user")];
		const _tstring& sPassword = mapParams[_T("password")];

		// Checking user credentials
		_tstring sPasswordHash;
		if (!Users::Instance().IsUserCredentialsValid(sLogin, sPassword, sPasswordHash))
		{
			THROW_EXCEPTION_CODE(cmn::CStatus::StatusCode::eUnauthorized, "Invalid credentials.");
		}

		// User authorized, generating session_hash
		CUserInfo::CUser User;
		User.ptLogin 		= boost::posix_time::second_clock().local_time();
		User.sLogin 		= sLogin;
		User.sPasswordHash 	= sPasswordHash;
		User.sSessionHash 	= cmn::CCrypto::GenerateRndString();

		// Setting up cookies
		Reply.SetCookieValue(_T("session_hash"), User.sSessionHash);

		// Getting user request path
		const _tstring& sRequestedPath = Info.GetCookieValue(_T("requested_path"));

		_tostringstream ossRedirectPage;
		ossRedirectPage << _T("<html><body><script>window.location = \"") << sRequestedPath << _T("\";</script></body></html>");

		Reply.AppendContent(ossRedirectPage.str());
	}
	catch(std::exception& e)
	{
		Logger::Instance().Write(_T("Authorization error."), cmn::ILog::Level::eDebug);
		Logger::Instance().Write(e.what(), cmn::ILog::Level::eDebug);

		_tostringstream ossRedirectPage;
		ossRedirectPage << _T("<html><body><script>window.location = \"login.html\";</script></body></html>");

		Reply.AppendContent(ossRedirectPage.str());
	}
}







