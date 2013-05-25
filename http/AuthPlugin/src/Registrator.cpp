/*
 * Registrator.cpp
 *
 *  Created on: Dec 17, 2011
 *      Author: root
 */

#include "Registrator.h"
#include "Singletones.h"
#include "Exception.h"
#include "UserInfo.h"
#include "Crypto.h"
#include "MailSender.h"
#include "Converter.h"

CRegistrator::CRegistrator()
{

}

const TCHAR *CRegistrator::Description()
{
	return _T("User registration executor.");
}

const bool CRegistrator::Match(const http::IHttpData & Request)
{
	// If request path is "/authorize" we should handle this request
	const _tstring& sPath = Request.GetRequestPath();
	if (sPath == _T("/register"))
	{
		return true;
	}
	return false;
}

void CRegistrator::Execute(const http::IHttpData & Request, http::IHttpData & Reply)
{
	// If params after '?' is empty sending registration form
	const http::IHttpData::TRequestParams& mapParams = Request.GetRequestParams();
	if (mapParams.empty())
	{
		_tostringstream ossFileName;
		ossFileName << Settings::Instance().GetDocRoot() << _T("register.html");
		Reply.AppendFileName(ossFileName.str());
		return;
	}

	// Check if registration_hash exists
	const http::IHttpData::TRequestParams::const_iterator it = mapParams.find(_T("registration_hash"));
	if (mapParams.end() != it)
	{
		const _tstring& sHash = it->second;

		// Gettings user info by this hash
		try
		{
			// Activation succeeded
			const CUserInfo::CUser& User = Users::Instance().Get(sHash);

			CUserInfo::CUser NewUserData;
			NewUserData.ptLogin 		= boost::posix_time::second_clock().local_time();
			NewUserData.sLogin 			= User.sLogin;
			NewUserData.sPasswordHash 	= User.sPasswordHash;
			NewUserData.sSessionHash 	= cmn::CCrypto::GenerateRndString();

			Users::Instance().Set(NewUserData.sSessionHash, NewUserData);

			// Setting up cookies
			Reply.SetCookieValue(_T("session_hash"), NewUserData.sSessionHash);

			// Getting user request path
			_tstring sRequestedPath = Request.GetCookieValue(_T("requested_path"));

			if (sRequestedPath.empty())
				sRequestedPath = _T("index.html");

			_tostringstream ossRedirectPage;
			ossRedirectPage << _T("<html><body><script>window.location = \"") << sRequestedPath << _T("\";</script></body></html>");

			Reply.AppendContent(ossRedirectPage.str());
			return;
		}
		catch(std::exception& e)
		{
			Logger::Instance().Write(e.what(), cmn::ILog::Level::eMessage);
			THROW_EXCEPTION_CODE(cmn::CStatus::StatusCode::eUnauthorized, _T("Bad registration hash."));
		}

	}

	// Sending email notification with generated hash
	const http::IHttpData::TRequestParams::const_iterator itLogin = mapParams.find(_T("login"));
	const http::IHttpData::TRequestParams::const_iterator itPassword = mapParams.find(_T("pass"));
	const http::IHttpData::TRequestParams::const_iterator itEmail = mapParams.find(_T("email"));

	const http::IHttpData::TRequestParams::const_iterator itEnd = mapParams.end();
	if (itEnd == itLogin || itEnd == itPassword || itEnd == itEmail)
	{
		THROW_EXCEPTION_CODE(cmn::CStatus::StatusCode::eUnauthorized, _T("Bad credentials."));
	}

	// Get user password hash
	std::vector< unsigned char > vecCredentials;
	vecCredentials.reserve(itLogin->second.size() + itPassword->second.size());

	std::copy(itLogin->second.begin(), itLogin->second.end(), std::back_inserter(vecCredentials));
	std::copy(itPassword->second.begin(), itPassword->second.end(), std::back_inserter(vecCredentials));

	const _tstring sPasswordHash = cmn::CCrypto::CalcMD5(&vecCredentials.front(), vecCredentials.size());

	CUserInfo::CUser NewUserData;
	NewUserData.ptLogin 		= boost::posix_time::second_clock().local_time();
	NewUserData.sLogin 			= itLogin->second;
	NewUserData.sPasswordHash 	= sPasswordHash;
	NewUserData.sSessionHash 	= sPasswordHash;

	Users::Instance().Set(_T(""), NewUserData);

	const _tstring sServer = Settings::Instance().GetValue(_T("server.email_server"));
	const _tstring sPort = Settings::Instance().GetValue(_T("server.email_port"));
	const _tstring sEmail = Settings::Instance().GetValue(_T("server.server_email_box"));
	const _tstring sTarget = itEmail->second;
	const _tstring sEncryptedPass = Settings::Instance().GetValue(_T("server.server_email_box_password"));

	_tstring sDecryptedPass;
	cmn::CCrypto Crypto(sEmail);
	Crypto.Decrypt(sEncryptedPass, sDecryptedPass);

	cmn::CMailSender Sender(sServer, sPort, sEmail, sDecryptedPass);

	_tostringstream ossMEssage;
	ossMEssage << _T("Follow this link to activate your account: ") << Settings::Instance().GetHost() << _T("/register?registration_hash=") << sPasswordHash << std::endl;
	Sender.Send(sTarget, ossMEssage.str());

	_tostringstream ossInforming;
	ossInforming << _T("<html><body>") << _T("Activation sended to: ") << itEmail->second << _T("</body></html>");

	Reply.AppendContent(ossInforming.str());

}

CRegistrator::~CRegistrator()
{
	// TODO Auto-generated destructor stub
}

