#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <string>

#include <boost/property_tree/ptree.hpp>

//! Класс настроек
class CSettings
{
public:
	CSettings(void);
	~CSettings(void);

	//! Загрузить настройки из файла
	void Load();

	//! Получить регулярное выражение для страницы
	const std::string GetPageExpression() const;

	//! Получить регулярное выражение для страницы
	const std::string GetItemExpression() const;

	//! Получить имя файла с результатами
	const std::string GetResultsFileName() const;

	//! Получить имя файла с телефонами
	const std::string GetPhonesFileName() const;

	//! Получить имя файла с агрегированными данными
	const std::string GetAggregatedFileName() const;

private:

	//! Сгенерировать стандартные настройки
	void MakeDefaultSettings();

	//! Получить полный путь тэга настроек
	const std::string GetCompletePath(const char * szTagName) const;

	//! XML файл с настройками
	boost::property_tree::ptree m_xmlData;

	//! Имя корневого тэга
	static const char * const SETTINGS_ROOT_NAME;

	//! Имя файла настроек
	static const char * const SETTINGS_FILE_NAME;

	//! Имя тэга для хранения регулярного выражения для страницы
	static const char * const TAG_NAME_PAGE_EXPRESSION;

	//! Имя тэга для хранения регулярного выражения для объявления
	static const char * const TAG_NAME_ITEM_EXPRESSION;

	//! Имена тэгов для хранения имен файлов
	static const char * const TAG_NAME_FILE_RESULTS; 
	static const char * const TAG_NAME_FILE_PHONES;
	static const char * const TAG_NAME_FILE_AGGREGATED;
};

//! Синглтон класса настроек
class Settings
{
public:
	static CSettings& Instance();
private:
	static CSettings ms_Settings;
};




#endif // SETTINGS_H_
