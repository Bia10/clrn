// XmlParser.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "XmlParser.h"

#include "File.h"
#include "Conversion.h"


CXmlParser::CXmlParser(void)
{
}


CXmlParser::~CXmlParser(void)
{
}


void CXmlParser::ParseFile(const _tstring& sFileName)
{
	m_Data.Childs.clear();
	m_Data.Data.clear();
	_tfstream ifs(clrn::CFile::GetCompletePath(sFileName).c_str(), std::ios_base::in);
	if (!ifs.is_open())
	{
		std::ostringstream oss;
#ifdef _UNICODE
		oss << _T("Can't open file! Path: ") << vr::str::narrow(clrn::CFile::GetCompletePath(sFileName), std::locale());
#elif
		oss << _T("Can't open file! Path: ") << clrn::CFile::GetCompletePath(sFileName);
#endif
		throw std::runtime_error(oss.str().c_str());
	}	

	std::vector< _tstring > Data;
	Data.assign(std::istream_iterator< _tstring, TCHAR > (ifs), std::istream_iterator< _tstring, TCHAR > ());
	
	_tstring sSource;
	std::vector< _tstring >::const_iterator it = Data.begin();
	const std::vector< _tstring >::const_iterator itEnd = Data.end();
	while (it != itEnd)
	{
		sSource.append(*it++);
		sSource.append(_T(" "));
	}


	Parse(sSource.c_str(), sSource.size());
}

void CXmlParser::Parse(const TCHAR * szSource, const long nLength)
{
	ParseTag(m_Data, const_cast< TCHAR * >(&szSource[0]), &szSource[nLength]);
}


void CXmlParser::GetTagAttributes(CXmlNode& Node, const _tstring& sTag)
{
	// У узла нет атрибутов
	if (sTag.find('=') == _tstring::npos)
		return;

	TCHAR * itStart = const_cast< TCHAR * > (&sTag[0]);
	const TCHAR * itEnd = &sTag[sTag.size()];

	while (itStart < itEnd)
	{
		// Ищем начало имени
		const TCHAR * itNameStart = std::find(static_cast< const TCHAR * > (itStart), itEnd, ' ') + 1;

		// Ищем разделитель "="
		const TCHAR * itSplitter = std::find(static_cast< const TCHAR * > (itStart), itEnd, '=');

		// Ищем окончание значения
		const TCHAR * itValueEnd = std::find(itSplitter + 2, itEnd, '\"');

		Node.Data[_tstring(itNameStart, itSplitter)] = _tstring(itSplitter + 2, itValueEnd);

		itStart = const_cast< TCHAR * > (itValueEnd + 1);
	}
}

const TCHAR * CXmlParser::ParseTag(CXmlNode& Node, TCHAR * pStart, const TCHAR * pEnd)
{
	while (pStart < pEnd)
	{
		// Ищем начало открывающего тэга
		const TCHAR * itStartTag	= std::find(static_cast< const TCHAR * > (pStart), pEnd, '<');
		const TCHAR * itStartTagName	= std::find_if(itStartTag, pEnd, boost::algorithm::is_alnum());

		// Нашли закрывающий тэг
		if (itStartTag == itStartTagName || *(itStartTag + 1) == '/')
			return std::find(const_cast< const TCHAR * > (pStart), pEnd, '>') + 1;

		// Ищем окончание открывающего тэга
		const TCHAR * itEndOpenTag	= std::find(itStartTag, pEnd, '>');

		// Ищем имя тэга
		const TCHAR * itEndName = std::find_if(itStartTagName, itEndOpenTag, !boost::algorithm::is_alnum());
		_tstring sTagName = _tstring(itStartTagName, itEndName);
		Node.Data[TAG_NAME] = sTagName;

		// Ищем тип тэга (?, <--)
		_tstring sTagType = _tstring(itStartTag + 1, itStartTagName);

		 //Находим все атрибуты тэга
		_tstring sTagData(itStartTagName, itEndOpenTag);
		if (!sTagType.empty())
			// Удаляем тип тэга из строки атрибутов
			boost::algorithm::replace_all(sTagData, sTagType, "");

		GetTagAttributes(Node, sTagData);

	/*if (!sTagType.empty())
		{

			std::find(const_cast< const TCHAR * > (pStart), pEnd, '>') + 1; 
		}*/

		// У тэга нет значения и вложенных тэгов
		if (*(itEndOpenTag - 1) == '/')
		{
			return itEndOpenTag + 1;
		}
		else
		{
			// Ищем вложенные узлы
			pStart = const_cast< TCHAR * > (std::find(itStartTagName, pEnd, '>'));
			while (true)
			{
				// Находим вложенные узлы и добавляем в Childs
				CXmlNode Child;
				pStart = const_cast< TCHAR * > (ParseTag(Child, pStart, pEnd));

				if (!Child.Data.empty())
					Node.Childs.push_back(Child);

				if (pStart >= pEnd)
					break;

				// Ищем закрывающий тэг </tag_name>
				TCHAR * itTagEnd = 0;
				while (true)
				{
					itTagEnd = const_cast< TCHAR * > (std::find(static_cast< const TCHAR * > (pStart), pEnd, '/'));
					if (*(itTagEnd - 1) == '<' || itTagEnd >= pEnd)
						break;
				}
				const TCHAR * itCloseTagNameEnd = std::find(static_cast< const TCHAR * > (itTagEnd), pEnd, '>');
				
				_tstring sCloseTagName;

				if (itTagEnd < itCloseTagNameEnd)
					sCloseTagName.assign(itTagEnd + 1, itCloseTagNameEnd);

				if (!sCloseTagName.empty() && sCloseTagName == sTagName)
				{
					_tstring sValue(pStart, itTagEnd - 1);
					if (!sValue.empty() && sValue != _T("\n"))
						Node.Data[TAG_VALUE] = sValue;
					break;
				}
			}	
		}
	}

	return pEnd;
}

void CXmlParser::Save(_tstring& sResult)
{
	WriteTag(m_Data, sResult, 0);
}

void CXmlParser::SaveToFile(const _tstring& sFileName)
{
	_tstring sResult;
	Save(sResult);
	_tstring sOut = _T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	_tofstream ofs(clrn::CFile::GetCompletePath(sFileName).c_str(), std::ios_base::out); 
	ofs << sResult;
}

void CXmlParser::WriteTag(CXmlNode& Node, _tstring& sResult, const int nNesting)
{
	sResult += GetNesting(nNesting);
	sResult += _T("<");
	sResult += Node.Data[TAG_NAME];
	{
		// Записываем все атрибуты
		CXmlNode::TData::const_iterator it = Node.Data.begin();
		const CXmlNode::TData::const_iterator itEnd = Node.Data.end();
		for (; it != itEnd; ++it)
		{
			if (it->first != TAG_NAME && it->first != TAG_VALUE)
			{
				sResult += _T(" ");
				sResult += it->first;
				sResult += _T("=\"");
				sResult += it->second;
				sResult += _T("\"");
			}
		}
	}
	sResult += _T(">");

	_tstring sRowValue = Node.Data[TAG_VALUE];
	if (!sRowValue.empty() && sRowValue != _T("\n"))
		sResult += sRowValue;

	{
		// Записываем все дочерние узлы
		CXmlNode::TChilds::iterator it = Node.Childs.begin();
		const CXmlNode::TChilds::const_iterator itEnd = Node.Childs.end();
		for (; it != itEnd; ++it)
		{
			sResult += _T("\n");
			WriteTag(*it, sResult, nNesting + 1);
		}
	}

	// Закрывающий тэг
	if (Node.Childs.size() > 1)
	{
		sResult += _T("\n");
		sResult += GetNesting(nNesting);
	}
	sResult += _T("</");
	sResult += Node.Data[TAG_NAME];
	sResult += _T(">");
}


void CXmlParser::TransformChildsToAttributes()
{
	CXmlNode NewData;
	TransformChildsToAttributes(m_Data, NewData);
	m_Data = NewData;
}

void CXmlParser::TransformChildsToAttributes(CXmlNode& OldNode, CXmlNode& NewNode)
{
	// Обход по всем дочерним узлам
	CXmlNode::TChilds::iterator it = OldNode.Childs.begin();
	const CXmlNode::TChilds::const_iterator itEnd = OldNode.Childs.end();
	for (; it != itEnd; ++it)
	{
		CXmlNode& Current = *it;
		NewNode.Data[TAG_NAME] = Current.Data[TAG_NAME];
		NewNode.Data[TAG_VALUE] = Current.Data[TAG_VALUE];
		CXmlNode::TChilds::iterator itChild = OldNode.Childs.begin();
		const CXmlNode::TChilds::const_iterator itChildEnd = OldNode.Childs.end();		
		for (; itChild != itChildEnd; ++itChild)
		{
			CXmlNode& CurrentChild = *itChild;

			// У узла только имя и значение
			if (CurrentChild.Childs.empty() && CurrentChild.Data.size() == 2 && CurrentChild.Data.count(TAG_VALUE))
			{
				NewNode.Data[CurrentChild.Data[TAG_NAME]] = CurrentChild.Data[TAG_VALUE];
			}
		}
		TransformChildsToAttributes(Current, NewNode);
	}
}


const _tstring CXmlParser::GetNesting(const int nNesting)
{
	_tstring sSpace;
	sSpace.resize(nNesting, ' ');
	return sSpace;
}


void CXmlParser::SetData(const CXmlNode& Data)
{
	m_Data = Data;
}

//! Получить данные
const CXmlNode& CXmlParser::GetData() const
{
	return m_Data;
}