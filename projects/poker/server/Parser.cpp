#include "Parser.h"
#include "Exception.h"

#include <iostream>

#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace srv
{

class Parser::Impl
{
public:
	Impl(ILog& logger) : m_Log(logger)
	{

	}

	bool Parse(const net::Packet& packet, Data& result)
	{
		return false;
	}

private:



private:

	//! Logger
	ILog&									m_Log;
};


Parser::Parser(ILog& logger) : m_Impl(new Impl(logger))
{

}

Parser::~Parser()
{
	delete m_Impl;
}

bool Parser::Parse(const net::Packet& packet, Data& result)
{
	return m_Impl->Parse(packet, result);
}



}