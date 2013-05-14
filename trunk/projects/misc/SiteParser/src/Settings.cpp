#include "Settings.h"

#include <iostream>
#include <sstream>

#include <boost/property_tree/xml_parser.hpp>

const char * const CSettings::SETTINGS_FILE_NAME = "settings.xml";
const char * const CSettings::SETTINGS_ROOT_NAME = "settings";
const char * const CSettings::TAG_NAME_PAGE_EXPRESSION = "page_expression";
const char * const CSettings::TAG_NAME_ITEM_EXPRESSION = "item_expression";
const char * const CSettings::TAG_NAME_FILE_RESULTS = "results_file";
const char * const CSettings::TAG_NAME_FILE_PHONES = "phones_file";
const char * const CSettings::TAG_NAME_FILE_AGGREGATED = "aggregated_file";

CSettings::CSettings(void)
{

}

CSettings::~CSettings(void)
{
}

const std::string CSettings::GetCompletePath(const char * szTagName) const
{
	std::ostringstream oss;
	oss << SETTINGS_ROOT_NAME << "." << szTagName;
	return oss.str();
}

void CSettings::Load()
{
	try
	{
		boost::property_tree::xml_parser::read_xml(SETTINGS_FILE_NAME, m_xmlData);
		std::cout << "Settings successfully loaded." << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << "Failed to load settings, loading defaults." << std::endl << e.what() << std::endl;
		MakeDefaultSettings();
		std::cout << "Saved settings to: " << SETTINGS_FILE_NAME << std::endl;
	}
}	

void CSettings::MakeDefaultSettings()
{
	m_xmlData.put(SETTINGS_ROOT_NAME, "");

	m_xmlData.put(GetCompletePath(TAG_NAME_PAGE_EXPRESSION), "<![CDATA[(<a name=\".*?\"></a>)(.*?)(<br>)]]>");
	m_xmlData.put(GetCompletePath(TAG_NAME_ITEM_EXPRESSION), "<![CDATA[^(\\d)?(-комн.\\s)?(\\S*)(,\\s)?(.*?, )?(.*?эт..*?, )?(.*?/у,)?(.*?\\.\\s)?(Оплата\\s)?((.*руб\\..*?\\.)?|(.*?\\.)?)?((.*?ник.*?\\.)?|(.*?Агентство.*?\\.)?)?(\\sТел.\\s.*)]]>");
	m_xmlData.put(GetCompletePath(TAG_NAME_FILE_RESULTS), "results");
	m_xmlData.put(GetCompletePath(TAG_NAME_FILE_PHONES), "phones");
	m_xmlData.put(GetCompletePath(TAG_NAME_FILE_AGGREGATED), "aggregated");

	boost::property_tree::xml_parser::xml_writer_settings< char > Settings = boost::property_tree::xml_parser::xml_writer_make_settings< char >(' ', 1, "windows-1251");
	boost::property_tree::xml_parser::write_xml(SETTINGS_FILE_NAME, m_xmlData, std::locale(), Settings);
}

const std::string CSettings::GetPageExpression() const
{
	return m_xmlData.get< std::string >(GetCompletePath(TAG_NAME_PAGE_EXPRESSION));
}

const std::string CSettings::GetItemExpression() const
{
	return m_xmlData.get< std::string >(GetCompletePath(TAG_NAME_ITEM_EXPRESSION));
}

const std::string CSettings::GetResultsFileName() const
{
	return m_xmlData.get< std::string >(GetCompletePath(TAG_NAME_FILE_RESULTS));
}

const std::string CSettings::GetPhonesFileName() const
{
	return m_xmlData.get< std::string >(GetCompletePath(TAG_NAME_FILE_PHONES));
}

const std::string CSettings::GetAggregatedFileName() const
{
	return m_xmlData.get< std::string >(GetCompletePath(TAG_NAME_FILE_AGGREGATED));
}

CSettings Settings::ms_Settings;

CSettings& Settings::Instance() 
{
	return ms_Settings;
}
