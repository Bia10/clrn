/*
 * UserInfo.cpp
 *
 *  Created on: Dec 17, 2011
 *      Author: root
 */

#include "UserInfo.h"
#include "Exception.h"
#include "Singletones.h"
#include "Crypto.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>

CUserInfo::CUserInfo(const _tstring& sUserInfoStorageFileName) :
	m_sStoragePath(sUserInfoStorageFileName)
{
	Load();
}

CUserInfo::~CUserInfo()
{

}

void CUserInfo::Set(const _tstring & sSessionId, const CUser & UserData)
{
	boost::mutex::scoped_lock Lock(m_mxData);

	if (!sSessionId.empty())
	{
		for (auto& User : m_vecUsers)
		{
			if (User.sSessionHash == sSessionId)
			{
				User = UserData;
				Store();
				return;
			}
		}
	}

	// Not found, adding
	m_vecUsers.push_back(UserData);
}

const CUserInfo::CUser & CUserInfo::Get(const _tstring & sSessionId) const
{
	boost::mutex::scoped_lock Lock(m_mxData);

	for (const auto& User : m_vecUsers)
	{
		if (User.sSessionHash == sSessionId)
		{
			return User;
		}
	}
	THROW_EXCEPTION(_T("Not found."));
}

void CUserInfo::Load()
{
	boost::mutex::scoped_lock Lock(m_mxData);

	boost::filesystem::path pthFile(m_sStoragePath);
	const _tstring sCompletePath = boost::filesystem::system_complete(pthFile).string();
	try
	{
		typedef boost::property_tree::ptree TXmlNode;

		TXmlNode xmlData;
		boost::property_tree::xml_parser::read_xml(sCompletePath, xmlData);

		const TXmlNode& xmlUsers = xmlData.get_child(_T("users"));
		for (const auto& xmlUser : xmlUsers)
		{
			CUser User;
			User.sLogin = xmlUser.second.get(_T("<xmlattr>.login"), _T(""));
			User.sPasswordHash = xmlUser.second.get(_T("<xmlattr>.password_hash"), _T(""));

			const _tstring sSessionId = cmn::CCrypto::GenerateRndString();

			Set(sSessionId, User);
		}
	}
	catch(std::exception& e)
	{
		Logger::Instance().Write(__FILE__, __LINE__, e.what(), cmn::ILog::Level::eError);
	}
}



const bool CUserInfo::IsUserCredentialsValid(const _tstring & sLogin, const _tstring & sPassword, _tstring& sPasswordHash) const
{
	if (sLogin.empty() || sPassword.empty())
		return false;

	// Get user password hash
	std::vector< unsigned char > vecCredentials;
	vecCredentials.reserve(sLogin.size() + sPassword.size());

	std::copy(sLogin.begin(), sLogin.end(), std::back_inserter(vecCredentials));
	std::copy(sPassword.begin(), sPassword.end(), std::back_inserter(vecCredentials));

	sPasswordHash = cmn::CCrypto::CalcMD5(&vecCredentials.front(), vecCredentials.size());

	boost::mutex::scoped_lock Lock(m_mxData);
	for (const auto& User : m_vecUsers)
	{
		if (!boost::algorithm::iequals(User.sLogin, sLogin))
			continue;

		if (!boost::algorithm::iequals(User.sPasswordHash, sPasswordHash))
			continue;

		return true;
	}

	return false;
}

void CUserInfo::Store() const
{
	boost::filesystem::path pthSettingsFile(m_sStoragePath);
	const _tstring sCompletePath = boost::filesystem::system_complete(pthSettingsFile).string();
	try
	{
		typedef boost::property_tree::ptree TXmlNode;

		TXmlNode xmlData;

		TXmlNode& xmlUsers = xmlData.put_child(_T("users"), TXmlNode());

		for (const auto& User : m_vecUsers)
		{
			TXmlNode xmlUser;
			xmlUser.put(_T("<xmlattr>.login"), 			User.sLogin);
			xmlUser.put(_T("<xmlattr>.password_hash"), 	User.sPasswordHash);

			xmlUsers.push_back(std::make_pair(_T("user"), xmlUser));
		}

		boost::property_tree::xml_writer_settings< TCHAR > Settings(' ', 1);
		boost::property_tree::xml_parser::write_xml(sCompletePath, xmlData, std::locale::classic(), Settings);
	}
	catch(std::exception& e)
	{
		Logger::Instance().Write(__FILE__, __LINE__, e.what(), cmn::ILog::Level::eError);
	}
}


boost::scoped_ptr< CUserInfo > Users::ms_pUsers;
void Users::Init(const _tstring & sUserInfoStorageFileName)
{
	ms_pUsers.reset(new CUserInfo(sUserInfoStorageFileName));
}

CUserInfo& Users::Instance()
{
	return *ms_pUsers;
}





