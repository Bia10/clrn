/*
 * TestCase.cpp
 *
 *  Created on: Dec 4, 2011
 *      Author: clrn
 */

#include "TestCase.h"

#include "Types.h"

#include <sstream>
#include <vector>

#include <boost/regex.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/property_tree/xml_parser.hpp>

CTestCase::CTestCase()
{
	// TODO Auto-generated constructor stub

}

CTestCase::~CTestCase()
{
	// TODO Auto-generated destructor stub
}

void CTestCase::ReadVariable(const CXmlNode & xmlNode)
{
	const _tstring& sName = xmlNode.get< _tstring >(_T("<xmlattr>.name"));
	_tstring sValue = xmlNode.data();
	CalculateVariables(sValue);

	m_mapVariables[sName] = sValue;
}



void CTestCase::Execute(const CXmlNode& m_xmlCaseData)
{
	CXmlNode::const_iterator it = m_xmlCaseData.begin();
	const CXmlNode::const_iterator itEnd = m_xmlCaseData.end();
	for (; it != itEnd; ++it)
	{
		if (it->first == _T("variable"))
		{
			ReadVariable(it->second);
		}
		else
		if (it->first == _T("query"))
		{
			DoQuery(it->second);
		}
		else
		if (it->first == _T("regexp"))
		{
			ParseRegex(it->second);
		}
		else
		if (it->first == _T("xml"))
		{
			ParseXml(it->second);
		}
	}
}

void CTestCase::DoQuery(const CXmlNode & xmlNode)
{
	_tstring sResult;
	_tstring sQueryText = xmlNode.get< _tstring >(_T("text"));
	CalculateVariables(sQueryText);
	if (sQueryText == _T("ACTION=AUTHORIZATION&USERID=10007937&USERNAME=CLRN"))
	{
		sResult = _T("role28");
	}
	else
	if (sQueryText == _T("ACTION=REPORTTREE&USERID=10007937&USERNAME=CLRN&ROLEID=28"))
	{
		sResult = _T("1\n2\n3\n");
	}
	else
	if (sQueryText == _T("ACTION=REPORTRUNINFO&USERID=10007937&USERNAME=CLRN&ROLEID=28&REPORTID=1"))
	{
		sResult = _T("<result><run id=\"100501\"></run></result>");
	}
	else
	if (sQueryText == _T("ACTION=REPORTRUNINFO&USERID=10007937&USERNAME=CLRN&ROLEID=28&REPORTID=2"))
	{
		sResult = _T("<result><run id=\"100502\"></run></result>");
	}
	else
	if (sQueryText == _T("ACTION=REPORTRUNINFO&USERID=10007937&USERNAME=CLRN&ROLEID=28&REPORTID=3"))
	{
		sResult = _T("<result><run id=\"100503\"></run></result>");
	}

	const boost::optional< const CXmlNode& > xmlResult = xmlNode.get_child_optional(_T("result"));
	if (!xmlResult)
		return;

	const _tstring& sVariableName = xmlResult.get().get< _tstring >(_T("<xmlattr>.name"));

	m_mapVariables[sVariableName] = sResult;

	Execute(xmlResult.get());
}

void CTestCase::ParseRegex(const CXmlNode & xmlNode)
{
	// Text of the regular expression
	const _tstring& sText = xmlNode.get< _tstring >(_T("text"));
	boost::basic_regex< TCHAR > rgxData(sText);

	// Input source
	_tstring sSource = xmlNode.get< _tstring >(_T("input"));
	CalculateVariables(sSource);

	const boost::optional< const CXmlNode& > xmlResult = xmlNode.get_child_optional(_T("result"));
	if (!xmlResult)
		return;

	// Determining output variables
	std::vector< _tstring > vecVariables;
	{
		const CXmlNode& xmlAttrs = xmlResult.get().get_child(_T("<xmlattr>"));

		CXmlNode::const_iterator it = xmlAttrs.begin();
		const CXmlNode::const_iterator itEnd = xmlAttrs.end();
		for (; it != itEnd; ++it)
		{
			const _tstring& sVariableName = it->second.data();
			if (sVariableName.empty())
				continue;

			vecVariables.push_back(sVariableName);
		}
	}

	boost::regex_iterator< _tstring::const_iterator > it(sSource.begin(), sSource.end(), rgxData);
	const boost::regex_iterator< _tstring::const_iterator > itEnd;
	for (; it != itEnd; ++it)
	{
		// For each variable getting match result
		for (std::size_t i = 0; i < vecVariables.size(); ++i)
		{
			const _tstring& sName = vecVariables[i];
			const _tstring& sValue = (*it)[i + 1];

			m_mapVariables[sName] = sValue;
		}

		// Execute stetements for each match
		Execute(xmlResult.get());
	}
}

void CTestCase::ParseXml(const CXmlNode & xmlNode)
{
	_tstring sInput = xmlNode.get< _tstring >(_T("input"));

	CalculateVariables(sInput);

	CXmlNode xmlParsedData;
	std::basic_istringstream< TCHAR > issXmlText(sInput);
	boost::property_tree::xml_parser::read_xml(issXmlText, xmlParsedData);

	// Executing tags
	ExecuteXmlTag(xmlNode, xmlParsedData);
}

void CTestCase::ExecuteXmlTag(const CXmlNode& xmlScript, const CXmlNode& xmlParsedData)
{
	CXmlNode::const_iterator it = xmlScript.begin();
	const CXmlNode::const_iterator itEnd = xmlScript.end();
	for (; it != itEnd; ++it)
	{
		if (it->first == _T("tag"))
		{
			const _tstring& sTagName = it->second.get< _tstring >(_T("<xmlattr>.name"));
			const CXmlNode& xmlTargetTag = xmlParsedData.get_child(sTagName);
			ExecuteXmlTag(it->second, xmlTargetTag);
		}
		else
		if (it->first == _T("variable"))
		{
			const _tstring& sVariableSource = it->second.data();
			const _tstring& sName = it->second.get< _tstring > (_T("<xmlattr>.name"));
			const _tstring& sValue = xmlParsedData.get< _tstring > (sVariableSource);
			m_mapVariables[sName] = sValue;
		}
		else
		if (it->first == _T("query"))
		{
			DoQuery(it->second);
		}
		else
		if (it->first == _T("regexp"))
		{
			ParseRegex(it->second);
		}
		else
		if (it->first == _T("xml"))
		{
			ParseXml(it->second);
		}
	}
}

void CTestCase::CalculateVariables(_tstring & sVariableData)
{
	static boost::basic_regex< TCHAR > s_rgxVariables(_T("\\{\\$(.*?)\\}"));

	boost::regex_iterator< _tstring::const_iterator > it(sVariableData.begin(), sVariableData.end(), s_rgxVariables);
	const boost::regex_iterator< _tstring::const_iterator > itEnd;

	TVariables mapReplaced;

	for (; it != itEnd; ++it)
	{
		const _tstring& sVariableName = (*it)[1];

		if (m_mapVariables.count(sVariableName))
		{
			std::ostringstream ossVariable;
			ossVariable << _T("{$") << sVariableName << _T("}");
			mapReplaced[ossVariable.str()] = m_mapVariables[sVariableName];
		}
	}

	{
		TVariables::const_iterator it = mapReplaced.begin();
		const TVariables::const_iterator itEnd = mapReplaced.end();
		for (; it != itEnd; ++it)
		{
			boost::algorithm::replace_all(sVariableData, it->first, it->second);
		}
	}
}



