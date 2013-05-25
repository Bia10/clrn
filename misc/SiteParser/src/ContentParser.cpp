/*
 * ContentParser.cpp
 *
 *  Created on: Aug 21, 2011
 *      Author: clrn
 */

#include "ContentParser.h"
#include "Downloader.h"
#include "Globals.h"
#include "Settings.h"

#include <iostream>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/locale.hpp>

/*
const char * const CContentParser::ms_szItemsExpression =
		"^(\\d)?" 												// кол-во комнат
		"(-комн.\\s)?"
		"(\\S*)"												// тип жилья
		"(,\\s)?"
		"(.*?, )?" 												// Улица/район
		"(.*?эт..*?, )?" 										// Этаж
		"(.*?/у,)?"  											// удобства
		"(.*?\\.\\s)?"  										// остальные условия
		"(Оплата\\s)?"
		"((.*руб\\..*?\\.)?|(.*?\\.)?)?"						// оплата
		"((.*?ник.*?\\.)?|(.*?Агентство.*?\\.)?)?"				// посредник
		"(\\sТел.\\s.*)"										// телефон
		;

const char * const CContentParser::ms_szPageExpression =
		"(<a name=\".*?\"></a>)"								// открывающие тэги
		"(.*?)"													// текст
		"(<br>)"												// закрывающие тэги
		;
		*/

const char * CContentParser::ms_pNames[] = {"rooms", "", "type", "", "location", "address", "comfort", "description", "", "cost", "cost", "realtor", "realtor", "realtor", "realtor", "phone", NULL};


CContentParser::CContentParser()
{
	// TODO Auto-generated constructor stub

}

CContentParser::~CContentParser()
{
	// TODO Auto-generated destructor stub
}

void CContentParser::ParsePhone(std::string& sSource)
{
	boost::algorithm::erase_all(sSource, "Тел. ");
	boost::algorithm::erase_all(sSource, "-");
	boost::algorithm::erase_all(sSource, " ");

	if (sSource.substr(0, 2) == "+7")
	{
		sSource.assign(&sSource.c_str()[1]);
		sSource[0] = '8';
	}

	boost::regex expression("(\\d*).");
	boost::sregex_iterator it(sSource.begin(), sSource.end(), expression);
	boost::sregex_iterator itEnd;

	for (; it != itEnd; ++it)
	{
		const std::string& sPhone = (*it)[1];
		if (sPhone.empty())
			continue;

		if (sPhone.size() < 5)
			continue;

		m_mapPhones[sPhone]++;
	}
}

void CContentParser::ParseItem(std::string& sSource, boost::property_tree::ptree& xmlResult)
{
	static boost::regex expression(Settings::Instance().GetItemExpression());

	// Контейнер для значений
	std::vector< std::string > vecRawValues;

	if (!boost::regex_split(std::back_inserter(vecRawValues), sSource, expression))
		throw std::runtime_error("Failed to parse item.");

	for (unsigned int i = 0; i < vecRawValues.size(); ++i)
	{
		std::string sName = ms_pNames[i];
		if (sName.empty())
			continue;

		std::string& sToken = vecRawValues[i];
		if (!sToken.empty())
		{
			if (sName == "phone")
				ParsePhone(sToken);
			else
			{
				boost::algorithm::trim(sToken);
				if (sToken[sToken.size() - 1] == ',')
					boost::algorithm::erase_last(sToken, ",");
			}

			xmlResult.put(sName, sToken);
		}
	}
}


void CContentParser::ParsePage(std::string& sSource, std::vector< std::string >& vecResult)
{
	static boost::regex expression(Settings::Instance().GetPageExpression());

	boost::sregex_iterator it(sSource.begin(), sSource.end(), expression);
	boost::sregex_iterator itEnd;

	for (; it != itEnd; ++it)
	{
		const std::string& sItem = (*it)[2];
		if (!sItem.empty())
			vecResult.push_back(sItem);
	}
}

bool CContentParser::ParseSite(const std::string& sURL, boost::property_tree::ptree& xmlResult)
{
	bool bNotEmpty = false;
	xmlResult.clear();
	boost::property_tree::ptree& xmlRoot = xmlResult.put("data", "");

	std::size_t nPageCount = 1;
	std::size_t nEmptyPages = 0;
	while(1)
	{
		try
		{
			const std::string sPage = (boost::format(sURL) % nPageCount++).str();

			std::cout << (boost::format("Processing page: [%s]...") % sPage).str();

			CDownloader Dwnldr;
			Dwnldr.Open(sPage);

			std::string sBuffer;
			Dwnldr.Read(sBuffer);

            const std::string::size_type encoding = sBuffer.find("charset=");
            if (encoding != std::string::npos)
            {
                const std::string::size_type encodingEnd = sBuffer.find("\"", encoding);
                if (encodingEnd != std::string::npos)
                {
                    const std::string encodingValue(sBuffer.substr(encoding + 8, encodingEnd - encoding - 8));

                    if (boost::algorithm::iequals(encodingValue, "utf-8"))
                    {
                        const std::wstring out = boost::locale::conv::utf_to_utf<wchar_t, char>(sBuffer);
                        sBuffer = boost::locale::conv::from_utf<wchar_t>(out, "cp1251");
                    }
                }
            }

			std::vector< std::string > vecItems;
			ParsePage(sBuffer, vecItems);

			std::cout << (boost::format("\titems parsed: [%s]") % vecItems.size()).str() << std::endl;

			if (vecItems.empty())
				++nEmptyPages;
			else
				nEmptyPages = 0;

			if (nEmptyPages > 5)
				break;

			bNotEmpty = true;

			std::vector< std::string >::iterator it = vecItems.begin();
			const std::vector< std::string >::iterator itEnd = vecItems.end();

			BOOST_FOREACH(std::string& sCurrent, vecItems)
			{
				xmlRoot.push_back(std::pair< std::string, boost::property_tree::ptree > ("item", boost::property_tree::ptree()));
				boost::property_tree::ptree& xmlItem = xmlRoot.back().second;
				ParseItem(sCurrent, xmlItem);
			}
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	return bNotEmpty;
}

void CContentParser::GetPhones(TPhones& mapPhones)
{
	mapPhones = m_mapPhones;
}

void CContentParser::GeneratePhonesXml(const TPhones& mpPhones, boost::property_tree::ptree& xmlResult)
{
	xmlResult.clear();
	boost::property_tree::ptree& xmlRoot = xmlResult.put(std::string(PHONES_ROOT_NAME), "");

	TPhones::const_iterator it = mpPhones.begin();
	const TPhones::const_iterator itEnd = mpPhones.end();
	for (; it != itEnd; ++it)
	{
		if (it->second < 2)
			continue;

		xmlRoot.push_back(std::make_pair(std::string(PHONES_ITEM_NAME), boost::property_tree::ptree()));
		boost::property_tree::ptree& xmlPhone = xmlRoot.back().second;
		xmlPhone.put(std::string("<xmlattr>.") + PHONES_NUMBER_NAME, it->first);
		xmlPhone.put(std::string("<xmlattr>.") + PHONES_COUNT_NAME, boost::lexical_cast< std::string > (it->second));
	}
}

