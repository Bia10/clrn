#pragma once

#include "stdafx.h"

#include <string>
#include <map>
#include <vector>
#include <fstream>

#include <boost/algorithm/string.hpp>


//! ��� ������ ��� �������� XML ��������
//! 
//! ��� ����		 "rowname"			="%raw_name%"  
//! �������� ����	 "rowvalue"			="%raw_name%"
//! ��������		 "%atribute_name%"	=%atribute_value%
//! 
//! ������: 
//! XML: <parameter id="1" name="account">P_ACCOUNT</parameter>
//! 
//! TXmlValue: TAG_NAME="parameter" "id"="1" "name"="account" TAG_VALUE="P_ACCOUNT"
//! 
typedef std::map< _tstring, _tstring > TXmlValue;
#define TAG_NAME _T("#rowname")
#define TAG_VALUE _T("#rowvalue")


//! ��������� ��� �������� XML ����
struct CXmlNode
{
	typedef TXmlValue TData;

	typedef std::vector< CXmlNode > TChilds;

	//! ����� ��� �� �������� ��������
	inline CXmlNode * FindNode(const _tstring sAttributeName, const _tstring sAttributeValue) 
	{
		if (Data.count(sAttributeName) && Data[sAttributeName] == sAttributeValue)
			return this;

		// ���������� ��� ��������� �����
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

	//! ����� ��� �� �����������
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

	//! ������ ����
	TXmlValue Data;

	//! �������� ����
	std::vector< CXmlNode > Childs;
};

class CXmlParser
{
public:
	CXmlParser(void);
	~CXmlParser(void);

	//! ���������� ������
	void Parse(const TCHAR * szSource, const long nLength);

	//! ���������� ����
	void ParseFile(const _tstring& sFileName);

	//! ��������� � ������
	void Save(_tstring& sResult);

	//! ��������� � ����
	void SaveToFile(const _tstring& sFileName);

	//! ������������� ���� � ����� ��������� � ��������
	void TransformChildsToAttributes();

	//! ���������� ������
	void SetData(const CXmlNode& Data);

	//! �������� ������
	const CXmlNode& GetData() const;

private:

	//! ���������� ���
	const TCHAR * ParseTag(CXmlNode& Node, TCHAR * pStart, const TCHAR * pEnd);

	//! �������� ��� � ������
	void WriteTag(CXmlNode& Node, _tstring& sResult, const int nNesting);

	//! �������� �������� ���� � ������
	const _tstring GetNesting(const int nNesting);

	//! �������� �������� ����
	void GetTagAttributes(CXmlNode& Node, const _tstring& sTagType);

	//! ������������� ���� � ����� ��������� � ��������
	void TransformChildsToAttributes(CXmlNode& OldNode, CXmlNode& NewNode);

	//! XML ������
	CXmlNode m_Data;
};

