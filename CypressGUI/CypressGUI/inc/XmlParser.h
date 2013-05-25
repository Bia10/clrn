#pragma once

#include "stdafx.h"

#include <string>
#include <map>
#include <vector>
#include <fstream>

#include <boost/algorithm/string.hpp>


//! Тип данных для хранения XML значений
//! 
//! имя узла		 "rowname"			="%raw_name%"  
//! значение узла	 "rowvalue"			="%raw_name%"
//! атрибуты		 "%atribute_name%"	=%atribute_value%
//! 
//! Пример: 
//! XML: <parameter id="1" name="account">P_ACCOUNT</parameter>
//! 
//! TXmlValue: TAG_NAME="parameter" "id"="1" "name"="account" TAG_VALUE="P_ACCOUNT"
//! 
typedef std::map< _tstring, _tstring > TXmlValue;
#define TAG_NAME _T("#rowname")
#define TAG_VALUE _T("#rowvalue")


//! Структура для хранения XML узла
struct CXmlNode
{
	typedef TXmlValue TData;

	typedef std::vector< CXmlNode > TChilds;

	//! Найти тэг по значению атрибута
	inline CXmlNode * FindNode(const _tstring sAttributeName, const _tstring sAttributeValue) 
	{
		if (Data.count(sAttributeName) && Data[sAttributeName] == sAttributeValue)
			return this;

		// Рекурсивно для вложенных узлов
		CXmlNode::TChilds::iterator it = Childs.begin();
		const CXmlNode::TChilds::const_iterator itEnd = Childs.end();
		for (; it != itEnd; ++it)
		{
			CXmlNode * pNode = it->FindNode(sAttributeName, sAttributeValue);
			if (pNode)
				return pNode;
		}
		return 0;
	}

	//! Найти тэг по содержимому
	inline CXmlNode * FindNode(const CXmlNode& Node)
	{
		if (Data == Node.Data)
			return this;

		CXmlNode::TChilds::iterator it = Childs.begin();
		const CXmlNode::TChilds::const_iterator itEnd = Childs.end();
		for (; it != itEnd; ++it)
		{
			CXmlNode * pNode = it->FindNode(Node);
			if (pNode)
				return pNode;
		}
		return 0;
	}

	//! Данные узла
	TXmlValue Data;

	//! Дочерние узлы
	std::vector< CXmlNode > Childs;
};

class CXmlParser
{
public:
	CXmlParser(void);
	~CXmlParser(void);

	//! Распарсить строку
	void Parse(const TCHAR * szSource, const long nLength);

	//! Распарсить файл
	void ParseFile(const _tstring& sFileName);

	//! Сохранить в строку
	void Save(_tstring& sResult);

	//! Сохранить в файл
	void SaveToFile(const _tstring& sFileName);

	//! Преобразовать узлы с одним значением в атрибуты
	void TransformChildsToAttributes();

	//! Установить данные
	void SetData(const CXmlNode& Data);

	//! Получить данные
	const CXmlNode& GetData() const;

private:

	//! Распарсить тэг
	const TCHAR * ParseTag(CXmlNode& Node, TCHAR * pStart, const TCHAR * pEnd);

	//! Записать тэг в строку
	void WriteTag(CXmlNode& Node, _tstring& sResult, const int nNesting);

	//! Получить смещение тэга в строке
	const _tstring GetNesting(const int nNesting);

	//! Получить атрибуты тэга
	void GetTagAttributes(CXmlNode& Node, const _tstring& sTagType);

	//! Преобразовать узлы с одним значением в атрибуты
	void TransformChildsToAttributes(CXmlNode& OldNode, CXmlNode& NewNode);

	//! XML данные
	CXmlNode m_Data;
};

