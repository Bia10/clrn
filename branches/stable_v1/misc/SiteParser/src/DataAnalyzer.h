/*
 * DataAnalyzer.h
 *
 *  Created on: Sep 5, 2011
 *      Author: clrn
 */

#ifndef DATAANALYZER_H_
#define DATAANALYZER_H_

#include "ContentParser.h"

#include <string>

#include <boost/property_tree/ptree_fwd.hpp>

class CDataAnalyzer
{
public:
	CDataAnalyzer();
	~CDataAnalyzer();

	//! Проанализировать папку с xml файлами, содержащими данные о телефонах
	void AnalyzePhones(const std::string& sDirectoryPath, CContentParser::TPhones& mpPhones);

private:

};

#endif /* DATAANALYZER_H_ */
