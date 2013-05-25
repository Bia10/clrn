/*
 * AuthPlugin.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: root
 */
#include "Factory.h"
#include "ILog.h"
#include "ISettings.h"

#include "Singletones.h"
#include "AuthChecker.h"
#include "Authorizer.h"
#include "Registrator.h"
#include "UserInfo.h"
#include "Crypto.h"
#include "Converter.h"


#ifdef __cplusplus
extern "C" {
#endif

void Init(cmn::TExecutorsFactory&, cmn::ILog&, cmn::ISettings&);
void Shutdown(void);

#ifdef __cplusplus
}
#endif


void Init(cmn::TExecutorsFactory& Factory, cmn::ILog& Logger, cmn::ISettings& Settings)
{
	Factory.Add< CRegistrator >(100);
	Factory.Add< CAuthorizer >(99);
	Factory.Add< CAuthChecker >(98);

	Logger::Init(Logger);
	Settings::Init(Settings);

	_tstring sStorageFileName;
	try
	{
		sStorageFileName = Settings.GetValue(_T("server.users_storage"));
	}
	catch(std::exception& e)
	{
		Logger::Instance().Write(__FILE__, __LINE__, e.what(), cmn::ILog::Level::eError);
		sStorageFileName = _T("userdata.xml");
		Settings.SetValue(_T("server.users_storage"), sStorageFileName);
		Settings.Save();
	}

	try
	{
		Settings.GetValue(_T("server.email_server"));
	}
	catch(std::exception& e)
	{
		Logger::Instance().Write(__FILE__, __LINE__, e.what(), cmn::ILog::Level::eError);

		_tstring sEmail;
		while (sEmail.empty())
		{
			std::cout << "Specify the email server please." << std::endl;
			std::cin >> sEmail;
		}

		Settings.SetValue(_T("server.email_server"), sEmail);
		Settings.Save();
	}

	try
	{
		Settings.GetValue(_T("server.email_port"));
	}
	catch(std::exception& e)
	{
		Logger::Instance().Write(__FILE__, __LINE__, e.what(), cmn::ILog::Level::eError);

		_tstring sEmailPort;
		while (sEmailPort.empty())
		{
			std::cout << "Specify the email server port please." << std::endl;
			std::cin >> sEmailPort;
		}

		Settings.SetValue(_T("server.email_port"), sEmailPort);
		Settings.Save();
	}

	_tstring sServerMailBox;
	try
	{
		sServerMailBox = Settings.GetValue(_T("server.server_email_box"));
	}
	catch(std::exception& e)
	{
		Logger::Instance().Write(__FILE__, __LINE__, e.what(), cmn::ILog::Level::eError);

		while (sServerMailBox.empty())
		{
			std::cout << "Specify the server email box please." << std::endl;
			std::cin >> sServerMailBox;
		}

		Settings.SetValue(_T("server.server_email_box"), sServerMailBox);
		Settings.Save();
	}

	_tstring sServerMailBoxPassword;
	try
	{
		sServerMailBoxPassword = Settings.GetValue(_T("server.server_email_box_password"));
	}
	catch(std::exception& e)
	{
		Logger::Instance().Write(__FILE__, __LINE__, e.what(), cmn::ILog::Level::eError);

		while (sServerMailBoxPassword.empty())
		{
			std::cout << "Specify the server email box password please." << std::endl;
			std::cin >> sServerMailBoxPassword;
		}

		cmn::CCrypto Crypto(sServerMailBox);

		_tstring sEncryptedPassword;
		Crypto.Encrypt(sServerMailBoxPassword, sEncryptedPassword);

		Settings.SetValue(_T("server.server_email_box_password"), sEncryptedPassword);
		Settings.Save();
	}

	Users::Init(sStorageFileName);
}

void Shutdown()
{

}
