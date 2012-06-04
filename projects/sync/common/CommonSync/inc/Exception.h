#ifndef Exception_h__
#define Exception_h__

#include "Config.h"

#include <exception>
#include <string>
#include <vector>

#include <boost/scoped_ptr.hpp>

//! Exception
//!
//! \class CExcept
//!
class CExcept
	: public std::exception
{
	CExcept(void);

public:

	CExcept(const CExcept& xcpt);

	//! Destructor
	~CExcept();

	//! Constructors
	CExcept(const char* szFile, const unsigned long line, const char* text);
	CExcept(const char* szFile, const unsigned long line, const std::string& text);

	//! Text
	const char* what() const throw();

	//! Append
	std::basic_ostream<char>& Append(const char* szFile, const unsigned long line);

	//! Append 
	std::basic_ostream<char>& Append();

private:

	//! Implementation
	class Impl; 
	boost::scoped_ptr<Impl> m_pImpl;
};

namespace xc
{

//! Type of the string vector
typedef std::vector<std::string>		StringVector;

//! Type of the string vector iterator
typedef StringVector::const_iterator	It;

//! Argument formatter
void FormatArguments(StringVector& args, const char* text = 0);

//! Message appenders
template<class T>
void Append(T& /*e*/, It /*it*/)
{

}
template<class T>
void Append(CExcept& e, It it, const T& arg1)
{
	e.Append() << "\t[" << *it << "] : [" << arg1 << "]" << std::endl;
}
template<class T1, class T2>
void Append(CExcept& e, It it, const T1& arg1, const T2& arg2)
{
	Append(e, it, arg1);
	Append(e, ++it, arg2);
}
template<class T1, class T2, class T3>
void Append(CExcept& e, It it, const T1& arg1, const T2& arg2, const T3& arg3)
{
	Append(e, it, arg1);
	Append(e, ++it, arg2);
	Append(e, ++it, arg3);
}
template<class T1, class T2, class T3, class T4>
void Append(CExcept& e, It it, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4)
{
	Append(e, it, arg1);
	Append(e, ++it, arg2);
	Append(e, ++it, arg3);
	Append(e, ++it, arg4);
}
template<class T1, class T2, class T3, class T4, class T5>
void Append(CExcept& e, It it, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5)
{
	Append(e, it, arg1);
	Append(e, ++it, arg2);
	Append(e, ++it, arg3);
	Append(e, ++it, arg4);
	Append(e, ++it, arg5);
}


} // namespace xc

#define APPEND_ARGS(xcpt, ...)							\
	xc::StringVector args;								\
	xc::FormatArguments(args, #__VA_ARGS__);			\
	xc::Append(xcpt, args.begin(), __VA_ARGS__);		\

#define TRY try
#define THROW(text) throw CMN_NAMESPACE_NAME::CExcept(__FILE__, __LINE__, text);

#define CATCH_PASS_CEXCEPT(...)														\
catch(CMN_NAMESPACE_NAME::CExcept& e)												\
{																					\
	e.Append(__FILE__, __LINE__);													\
	APPEND_ARGS(e, __VA_ARGS__); throw;												\
}

#define CATCH_PASS_STDEXCEPT(...)													\
catch(const std::exception& e)														\
{																					\
	CMN_NAMESPACE_NAME::CExcept except(__FILE__, __LINE__, e.what());				\
	APPEND_ARGS(except, __VA_ARGS__); throw except;									\
}

#define CATCH_PASS_UNHANDLED(...)													\
catch(...)																			\
{																					\
	CMN_NAMESPACE_NAME::CExcept except(__FILE__, __LINE__, "Unhandled exception.");	\
	APPEND_ARGS(except, __VA_ARGS__); throw except;									\
}

#define CATCH_IGNORE_CEXCEPT(log, ...)												\
catch(CMN_NAMESPACE_NAME::CExcept& e)												\
{																					\
	e.Append(__FILE__, __LINE__);													\
	APPEND_ARGS(e, __VA_ARGS__);													\
	log.Error("Ignored: %s") % e.what();											\
}

#define CATCH_IGNORE_STDEXCEPT(log, ...)											\
catch(const std::exception& e)														\
{																					\
	CMN_NAMESPACE_NAME::CExcept except(__FILE__, __LINE__, e.what());				\
	APPEND_ARGS(except, __VA_ARGS__);												\
	log.Error("Ignored: %s") % except.what();										\
}

#define CATCH_IGNORE_UNHANDLED(log, ...)											\
catch(...)																			\
{																					\
	CMN_NAMESPACE_NAME::CExcept except(__FILE__, __LINE__, "Unhandled exception.");	\
	APPEND_ARGS(except, __VA_ARGS__);												\
	log.Error("Ignored: %s") % except.what();										\
}

#define CATCH_PASS_EXCEPTIONS(...)					\
	CATCH_PASS_CEXCEPT(__VA_ARGS__)					\
	CATCH_PASS_STDEXCEPT(__VA_ARGS__)				\
	CATCH_PASS_UNHANDLED(__VA_ARGS__)		

#define CATCH_IGNORE_EXCEPTIONS(log, ...)			\
	CATCH_IGNORE_CEXCEPT(log, __VA_ARGS__)			\
	CATCH_IGNORE_STDEXCEPT(log, __VA_ARGS__)		\
	CATCH_IGNORE_UNHANDLED(log, __VA_ARGS__)

#define CHECK(expr, ...)																\
	if (!(expr))																		\
	{																					\
		CMN_NAMESPACE_NAME::CExcept except(__FILE__, __LINE__, "(" #expr ") failed.");	\
		APPEND_ARGS(except, __VA_ARGS__);												\
		throw except;																	\
	}

#endif // Exception_h__