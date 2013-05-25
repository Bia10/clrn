#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <string>

#include <boost/property_tree/ptree.hpp>

//! ����� ��������
class CSettings
{
public:
	CSettings(void);
	~CSettings(void);

	//! ��������� ��������� �� �����
	void Load();

	//! �������� ���������� ��������� ��� ��������
	const std::string GetPageExpression() const;

	//! �������� ���������� ��������� ��� ��������
	const std::string GetItemExpression() const;

	//! �������� ��� ����� � ������������
	const std::string GetResultsFileName() const;

	//! �������� ��� ����� � ����������
	const std::string GetPhonesFileName() const;

	//! �������� ��� ����� � ��������������� �������
	const std::string GetAggregatedFileName() const;

private:

	//! ������������� ����������� ���������
	void MakeDefaultSettings();

	//! �������� ������ ���� ���� ��������
	const std::string GetCompletePath(const char * szTagName) const;

	//! XML ���� � �����������
	boost::property_tree::ptree m_xmlData;

	//! ��� ��������� ����
	static const char * const SETTINGS_ROOT_NAME;

	//! ��� ����� ��������
	static const char * const SETTINGS_FILE_NAME;

	//! ��� ���� ��� �������� ����������� ��������� ��� ��������
	static const char * const TAG_NAME_PAGE_EXPRESSION;

	//! ��� ���� ��� �������� ����������� ��������� ��� ����������
	static const char * const TAG_NAME_ITEM_EXPRESSION;

	//! ����� ����� ��� �������� ���� ������
	static const char * const TAG_NAME_FILE_RESULTS; 
	static const char * const TAG_NAME_FILE_PHONES;
	static const char * const TAG_NAME_FILE_AGGREGATED;
};

//! �������� ������ ��������
class Settings
{
public:
	static CSettings& Instance();
private:
	static CSettings ms_Settings;
};




#endif // SETTINGS_H_
