/*
 * TestCase.h
 *
 *  Created on: Dec 4, 2011
 *      Author: clrn
 */

#ifndef TESTCASE_H_
#define TESTCASE_H_

#include "Types.h"

#include <map>

#include <boost/property_tree/ptree.hpp>

class CTestCase
{
private:
	typedef boost::property_tree::ptree CXmlNode;

	typedef std::map< _tstring, _tstring > TVariables;
public:
	CTestCase();
	virtual ~CTestCase();

	void Execute(const CXmlNode& xmlCase);

private:
	void ReadVariable(const CXmlNode& xmlNode);

	void DoQuery(const CXmlNode& xmlNode);

	void ParseRegex(const CXmlNode& xmlNode);

	void ParseXml(const CXmlNode& xmlNode);

	void ExecuteXmlTag(const CXmlNode& xmlScript, const CXmlNode& xmlParsedData);

	void CalculateVariables(_tstring& sVariableData);

	TVariables				m_mapVariables;
};

#endif /* TESTCASE_H_ */
