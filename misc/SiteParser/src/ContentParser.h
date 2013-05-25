/*
 * ContentParser.h
 *
 *  Created on: Aug 21, 2011
 *      Author: clrn
 */

#ifndef CONTENTPARSER_H_
#define CONTENTPARSER_H_

#include <string>
#include <vector>
#include <map>

#include <boost/property_tree/ptree.hpp>

//! \class CContentParser
//!
//! Объект для распарсивания html страниц
//!
class CContentParser
{
public:
	CContentParser();
	~CContentParser();

	//! Тип списка телефонов
	typedef std::map< std::string, int > TPhones;

	//! Распарсить сайт
	bool ParseSite(const std::string& sURL, boost::property_tree::ptree& xmlResult);

	//! Получить список телефонов
	void GetPhones(TPhones& mpPhones);

	//! Сгенерировать xml со списком телефонов
	void GeneratePhonesXml(const TPhones& mpPhones, boost::property_tree::ptree& xmlResult);

private:

	//! Распарсить содержимое объявления
	void ParseItem(std::string& sSource, boost::property_tree::ptree& xmlResult);

	//! Распарсить содержимое страницы
	void ParsePage(std::string& sSource, std::vector< std::string >& vecResult);

	//! Распарсить телефонный номер
	void ParsePhone(std::string& sPhone);

	//! Телефоны и сколько раз встречаются
	TPhones m_mapPhones;

	//! Имена токенов
	static const char * ms_pNames[];


};

#endif /* CONTENTPARSER_H_ */
