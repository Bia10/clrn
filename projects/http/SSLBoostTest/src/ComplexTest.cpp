/*
 * CComplexTest.cpp
 *
 *  Created on: Dec 4, 2011
 *      Author: clrn
 */

#include "ComplexTest.h"
#include "TestCase.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

CComplexTest::CComplexTest()
{
	// TODO Auto-generated constructor stub

}

void CComplexTest::Do(const _tstring & sFileName)
{
	CXmlNode xmlData;
	boost::property_tree::xml_parser::read_xml(sFileName, xmlData);

	const CXmlNode& xmlCase = xmlData.get_child(_T("complex_test.cases.case"));

	CTestCase Case;
	Case.Execute(xmlCase);

}

CComplexTest::~CComplexTest()
{
	// TODO Auto-generated destructor stub
}

