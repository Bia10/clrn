#ifndef IStatement_h__
#define IStatement_h__

#include "Recordset.h"

#include <string>

#include <boost/shared_ptr.hpp>

namespace sql
{

//! NULL field value
class Null {};

//! Statement abstraction
class IStatement
{
public:

	typedef boost::shared_ptr<IStatement> Ptr;

	virtual ~IStatement() {}
	virtual unsigned int Execute() = 0;
	virtual IStatement& operator << (const std::string& value) = 0;
	virtual IStatement& operator << (const char* value) = 0;
	virtual IStatement& operator << (const double value) = 0;
	virtual IStatement& operator << (const int value) = 0;
	virtual IStatement& operator << (const unsigned int value) = 0;
	virtual IStatement& operator << (const Recordset::Data& value) = 0;
	virtual IStatement& operator << (Null) = 0;
	virtual IStatement& operator >> (std::string& value) = 0;
	virtual IStatement& operator >> (char* value) = 0;
	virtual IStatement& operator >> (double& value) = 0;
	virtual IStatement& operator >> (unsigned int& value) = 0;
	virtual IStatement& operator >> (int& value) = 0;
	virtual IStatement& operator >> (Recordset::Data& value) = 0;
	virtual IStatement& operator >> (Null) = 0;
};
}


#endif // IStatement_h__
