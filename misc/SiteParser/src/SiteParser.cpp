//============================================================================
// Name        : SiteParser.cpp
// Author      : CLRN
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <sstream>

#include "Downloader.h"
#include "ContentParser.h"
#include "DataAnalyzer.h"
#include "Settings.h"

#include <boost/regex.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

static const char * const HELP_INFO =
		"Site parser. Made by CLRN. clrnmail@gmail.com.\n"
		"usage: <command> <arg>\n"
		"commands:\n"
		"parse - parse site, arg - url with %s determining page\n"
		"\tExample: parse www.domen.ru/page=%s\n"
		"aggr - aggregate phone data, arg - directory with xml files\n"
		"\tExample: aggr ../../data\n"
		"\tExample: aggr C:/usr/data\n"
		;

static const char * const CMD_HELP = "help";
static const char * const CMD_PARSE = "parse";
static const char * const CMD_AGGR = "aggr";

namespace fs = boost::filesystem;


const std::string MakeCompleteFileName(const char * szFileName)
{
	std::ostringstream oss;
	boost::posix_time::ptime ptTime = boost::posix_time::second_clock::local_time();

	oss << szFileName << "_" << boost::algorithm::replace_all_copy(boost::posix_time::to_iso_extended_string(ptTime), ":", "_") << ".xml";

	fs::path pthFile(oss.str());
	fs::path pthFull = fs::system_complete(pthFile);
	return pthFull.string();
}

int main(const int nArg, const char * pArgs[])
{
	try
	{
		Settings::Instance().Load();

		if (nArg == 1 || (nArg == 2 && strcmp(CMD_HELP, pArgs[1]) == 0))
		{
			std::cout << HELP_INFO << std::endl;
			throw std::runtime_error("exit.");
		}

		if (nArg < 3)
		{
			throw std::runtime_error("3 arguments expected");
		}

		std::cout << (boost::format("Command: [%s]. Argument: [%s].") % pArgs[1] % pArgs[2]).str() << std::endl;

		if (strcmp(CMD_PARSE, pArgs[1]) == 0)
		{
			CContentParser Parser;

			boost::property_tree::ptree xmlResult;

			if (!Parser.ParseSite(pArgs[2], xmlResult))
				throw std::runtime_error("Not found any matches. Result is empty.");

			const std::string sItems = MakeCompleteFileName(Settings::Instance().GetResultsFileName().c_str());
			std::cout << (boost::format("Saving results to: [%s].") % sItems).str() << std::endl;
			boost::property_tree::xml_parser::write_xml(sItems, xmlResult, std::locale(), boost::property_tree::xml_parser::xml_writer_make_settings< char >(' ', 1, "windows-1251"));

			CContentParser::TPhones mpPhones;
			Parser.GetPhones(mpPhones);
			Parser.GeneratePhonesXml(mpPhones, xmlResult);

			const std::string sPhones = MakeCompleteFileName(Settings::Instance().GetPhonesFileName().c_str());
			std::cout << (boost::format("Saving phones to: [%s].") % sPhones).str() << std::endl;
			boost::property_tree::xml_parser::write_xml(sPhones, xmlResult, std::locale(), boost::property_tree::xml_parser::xml_writer_make_settings< char >(' ', 1, "windows-1251"));
		}
		else if (strcmp(CMD_AGGR, pArgs[1]) == 0)
		{
			CDataAnalyzer Analyzer;

			CContentParser::TPhones mpPhones;

			Analyzer.AnalyzePhones(pArgs[2], mpPhones);
			boost::property_tree::ptree xmlResult;

			CContentParser Parser;
			Parser.GeneratePhonesXml(mpPhones, xmlResult);

			const std::string sAggr = MakeCompleteFileName(Settings::Instance().GetAggregatedFileName().c_str());
			std::cout << (boost::format("Saving aggregated phone data to: [%s].") % sAggr).str() << std::endl;
			boost::property_tree::xml_parser::write_xml(sAggr, xmlResult, std::locale(), boost::property_tree::xml_parser::xml_writer_make_settings< char >(' ', 1, "windows-1251"));
		}
		else
			throw std::runtime_error("unknown command!");
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	system("pause");

	return 0;
}
