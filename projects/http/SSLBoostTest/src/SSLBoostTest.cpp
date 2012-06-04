//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>

#include <boost/shared_ptr.hpp>

class CAbstract
{
	virtual ~CAbstract()
	{

	}
	short b;
};

class CFoo
{
public:

	CFoo()
	{

	}

	virtual ~CFoo()
	{
		throw "das";
	}

	//bool i;
	//int m;
};

class CFoo1
{
public:

	CFoo1()
	{

	}

	virtual ~CFoo1()
	{
		throw "das";
	}

	//bool i;
	//int m;
};


class CBar : public CFoo, CFoo1
{
	virtual ~CBar()
	{

	}
};

int ones(unsigned int i)
{
	int j = 0;
	while (i)
	{
		int nTemp = i - 1;
		i &= nTemp;
		++j;
	}
	return j;
}

#include <limits.h>
#include <math.h>
#include <stdio.h>

std::string bytesToHex(char * bytes, size_t size)
{
    std::string result;
    for (int i=0; i<size; i++) {
        char hexByte[2];
        sprintf(hexByte, "%02x", static_cast< int >(bytes[i]));
        std::cout << std::hex << static_cast< int >(bytes[i]);
        result.append(hexByte);
    }

    std::cout << std::endl;
    return result;
}

int main(int argc, char* argv[])
{
	std::cout << bytesToHex("erwq123", 7) << std::endl;


	{
		int i=0;
		++i;
		i++;
		int *p = &i;
		int nTest = ++*p++;
		std::cout << nTest << std::endl;
		std::cout << *p << std::endl;
		//++i++;
	}

	std::cout << sizeof(CAbstract) << std::endl;

	int nMax = INT_MAX;
	int nMin = INT_MIN;

	int nTest = pow(2, 16);

	int i = 3217;

	std::cout << ones(i) << std::endl;


	unsigned int nCount = 0;
	for (unsigned int n = 0; n < sizeof(i) * 8; ++n)
	{
		bool bIsOne = i & 1;
		if (bIsOne)
			++nCount;
		i >>= 1;
		std::cout << i << std::endl;
	}


	std::cout << i << std::endl;
	std::cout << i << std::endl;
	std::cout << i << std::endl;
	std::cout << i << std::endl;
	std::cout << i << std::endl;


	CFoo Foo;

	std::cout << sizeof(CFoo) << std::endl;
	std::cout << sizeof(CBar) << std::endl;

	const char * szTest = 0;
	try
	{
		boost::shared_ptr< std::string > p1(new std::string("1"));
		boost::shared_ptr< std::string > p2(new std::string("2"));

		p1 = p2;

	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	szTest = "aaaa";

	return 0;
}
