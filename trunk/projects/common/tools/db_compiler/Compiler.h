#ifndef Compiler_h__
#define Compiler_h__

#include "ILog.h"

//! Class for db compile/script executing
//!
//! \class CDBCompiler
//!
class CDBCompiler
{
	void operator = (CDBCompiler&);
public:
	CDBCompiler(ILog& logger);
	~CDBCompiler(void);

	//! Create DB file
	void Create(const std::string& fileName);

	//! Open database
	void Open(const std::string& fileName);

	//! Execute script from file
	void ExecuteFile(const std::string& fileName);

private:

	//! Logger
	ILog&	m_Logger;
};
#endif // Compiler_h__
