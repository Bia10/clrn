/*
 * DataAnalyzer.cpp
 *
 *  Created on: Sep 5, 2011
 *      Author: clrn
 */

#include "DataAnalyzer.h"
#include "Globals.h"

#include <map>
#include <sstream>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/format.hpp>

namespace fs = boost::filesystem;

CDataAnalyzer::CDataAnalyzer()
{
	// TODO Auto-generated constructor stub

}

CDataAnalyzer::~CDataAnalyzer()
{
	// TODO Auto-generated destructor stub
}

void CDataAnalyzer::AnalyzePhones(const std::string& sDirectoryPath, CContentParser::TPhones& mpPhones)
{
	fs::path pthDirectory(sDirectoryPath);
	fs::path ptFull = fs::system_complete(pthDirectory);

	if (!fs::exists(ptFull))
	{
		std::ostringstream oss;
		oss << "Path " << sDirectoryPath << " does not exists!" << std::endl;
		throw std::runtime_error(oss.str().c_str());
	}

	boost::filesystem::recursive_directory_iterator itDir(ptFull);
	boost::filesystem::recursive_directory_iterator itDirEnd;

	std::size_t nFilesCount = 0;
	for (; itDir != itDirEnd; ++itDir)
	{
		fs::path pthCurrent(*itDir);

		if (!fs::is_directory(pthCurrent))
		{
			// Определяем расширение файла, интересует только xml
			const std::string sExtension = fs::extension(pthCurrent);
			if (sExtension != ".xml")
				continue;

			try
			{
				std::cout << (boost::format("Processing file: [%s].") % pthCurrent.string()).str() << std::endl;

				boost::property_tree::ptree xmlCurrent;
				boost::property_tree::xml_parser::read_xml(pthCurrent.string(), xmlCurrent, 0, std::locale());

				boost::property_tree::ptree& xmlRoot = xmlCurrent.get_child(std::string(PHONES_ROOT_NAME));

				boost::property_tree::ptree::iterator it = xmlRoot.begin();
				const boost::property_tree::ptree::const_iterator itEnd = xmlRoot.end();
				for (; it != itEnd; ++it)
				{
					boost::property_tree::ptree::value_type& xmlCurrent = *it;


					if (xmlCurrent.first != PHONES_ITEM_NAME)
						continue;

					const std::string& sPhone = xmlCurrent.second.get(std::string("<xmlattr>.") + PHONES_NUMBER_NAME, "");
					const int nCount = xmlCurrent.second.get< int >(std::string("<xmlattr>.") + PHONES_COUNT_NAME);

					if (sPhone.empty())
						continue;

					if (mpPhones[sPhone] < nCount)
						mpPhones[sPhone] = nCount;
				}

				++nFilesCount;
			}
			catch(std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}
		}
	}

}
