/*
 * ServerSettings.cpp
 *
 *  Created on: Nov 15, 2011
 *      Author: root
 */

#include "ServerSettings.h"

#include <algorithm>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem/operations.hpp>

namespace cmn
{

CServerSettings::CServerSettings() :
		m_pxmlSettings(new TXmlNode())
{


}

CServerSettings::~CServerSettings()
{

}

void CServerSettings::Load(const _tstring& sSettingsFileName)
{
	boost::filesystem::path pthSettingsFile(sSettingsFileName);
	m_sPath = boost::filesystem::system_complete(pthSettingsFile).string();
	try
	{
		boost::property_tree::xml_parser::read_xml(m_sPath, *m_pxmlSettings);
		std::cout << _T("Settings loaded from: ") << m_sPath << std::endl;
		
		// Correcting pathes
		const boost::filesystem::path pthFile(m_pxmlSettings->get(_T("server.www_root"), _T("")));
		m_pxmlSettings->put(_T("server.www_root"), boost::filesystem::system_complete(pthFile).string());
	}
	catch(std::exception& e)
	{
		std::cout
			<< _T("Failed to load settings. ") << e.what() << std::endl
			<< _T("Creating default settings file: ") << m_sPath << std::endl;
		CreateDefault();
	}

	std::cout
		<< _T("Host:\t\t\t") 		<< GetHost() 									<< std::endl
		<< _T("Port:\t\t\t") 		<< GetPort() 									<< std::endl
		<< _T("Threads:\t\t") 		<< GetNumberOfThreads() 						<< std::endl
		<< _T("Log source:\t\t") 	<< GetLogSource() 								<< std::endl
		<< _T("Log level:\t\t") 	<< static_cast< std::size_t > (GetLogLevel()) 	<< std::endl
		<< _T("Www root:\t\t") 		<< GetDocRoot() 								<< std::endl
		<< _T("Plugins path:\t\t") 	<< GetPluginsPath() 							<< std::endl
		<< _T("Plugins:\t\t");

	std::vector< _tstring > vecPlugins = GetPluginsList();
	std::copy(vecPlugins.begin(), vecPlugins.end(), std::ostream_iterator< _tstring >(std::cout, _T(", ")));

	std::cout << std::endl;
}

const _tstring CServerSettings::GetLogSource() const
{
	return m_pxmlSettings->get(_T("server.log.source"), _T(""));
}

const ILog::Level CServerSettings::GetLogLevel() const
{
	return static_cast< cmn::ILog::Level > (m_pxmlSettings->get< std::size_t >(_T("server.log.level"), 0));
}

const _tstring CServerSettings::GetHost() const
{
	return m_pxmlSettings->get(_T("server.host"), _T(""));
}

const std::size_t CServerSettings::GetPort() const
{
	return m_pxmlSettings->get< std::size_t >(_T("server.port"), 0);
}

const std::size_t CServerSettings::GetNumberOfThreads() const
{
	return m_pxmlSettings->get< std::size_t >(_T("server.threads"), 0);
}

const _tstring CServerSettings::GetDocRoot() const
{
	return m_pxmlSettings->get(_T("server.www_root"), _T(""));
}

const _tstring CServerSettings::GetPluginsPath() const
{
	return m_pxmlSettings->get(_T("server.plugins.<xmlattr>.path"), _T(""));
}

const std::vector<_tstring> CServerSettings::GetPluginsList() const
{
	std::vector<_tstring> vecResult;

	const TXmlNode& xmlPlugins = m_pxmlSettings->get_child(_T("server.plugins"));
	for (const auto& xmlPlugin : xmlPlugins)
	{
		if (xmlPlugin.first == _T("<xmlattr>"))
			continue;

		vecResult.push_back(xmlPlugin.second.data());
	}

	return vecResult;
}

const _tstring CServerSettings::GetValue(const _tstring & sPath) const
{
	assert(m_pxmlSettings);

	return m_pxmlSettings->get< _tstring > (sPath);
}

void CServerSettings::SetValue(const _tstring & sPath, const _tstring & sValue)
{
	assert(m_pxmlSettings);

	m_pxmlSettings->put(sPath, sValue);
}

void CServerSettings::CreateDefault()
{
	m_pxmlSettings->put(_T("server.log.source"), _T("std::cout"));
	m_pxmlSettings->put(_T("server.log.level"), 0);
	m_pxmlSettings->put(_T("server.host"), _T("127.0.0.1"));
	m_pxmlSettings->put(_T("server.port"), 80);
	m_pxmlSettings->put(_T("server.threads"), 5);
	m_pxmlSettings->put(_T("server.www_root"), _T("/"));
	m_pxmlSettings->put(_T("server.plugins.<xmlattr>.path"), _T("/opt/clrnsrv/plugins/"));

	Save();
}

void CServerSettings::Save()
{
	boost::property_tree::xml_writer_settings< TCHAR > Settings(' ', 1);
	boost::property_tree::xml_parser::write_xml(m_sPath, *m_pxmlSettings, std::locale::classic(), Settings);
}


CServerSettings Settings::ms_Settings;
void Settings::Open(const _tstring& sSettingsFileName)
{
	ms_Settings.Load(sSettingsFileName);
}

CServerSettings & Settings::Instance()
{
	return ms_Settings;
}

    /* namespace cmn */
}

