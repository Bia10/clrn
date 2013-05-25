/*
 * CComplexTest.h
 *
 *  Created on: Dec 4, 2011
 *      Author: clrn
 */

#ifndef CCOMPLEXTEST_H_
#define CCOMPLEXTEST_H_

#include "Types.h"

#include <boost/property_tree/ptree_fwd.hpp>

class CComplexTest
{
private:

	typedef boost::property_tree::ptree CXmlNode;
public:
	CComplexTest();

	void Do(const _tstring& sFileName);

	virtual ~CComplexTest();
};

#endif /* CCOMPLEXTEST_H_ */
