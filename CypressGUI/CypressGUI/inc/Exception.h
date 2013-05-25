//! Include project headers
#include "stdafx.h"

//! Include STL library headers
#include <exception>
#include <string>
#include "Conversion.h"

//! Include Boost library headers
#include <boost/format.hpp>
#include <boost/thread.hpp>

#ifndef _Exception_H_Included
#define _Exception_H_Included

namespace clrn{

//! Exception implementation
class CException : public std::exception
{
private:
	//! Not implemented
	CException();

public:

	//! Constructor
	CException(const char * const szFile, const long nLine, const char * szMessage) :
		std::exception(szMessage)
	{
		m_sMessage = (boost::format("%s\nFile: %s\nLine: %s\n") % szMessage % szFile % nLine).str();
	}

	//! Constructor
	CException(const char * const szFile, const long nLine, const std::string& sMessage) :
		std::exception(sMessage.c_str())
	{
		m_sMessage = (boost::format("%s\nFile: %s\nLine: %s\n") % sMessage % szFile % nLine).str();
	}

	//! Destructor
	virtual ~CException()
	{

	}

	//! Get message
	virtual const char * what() const
	{
		return m_sMessage.c_str();
	}

private:

	//! Message
	std::string							m_sMessage;
};

//! User message implementation
class CMessage : public std::exception
{
public:
	//! Constructor
	CMessage(const char * szMessage) :
		std::exception(szMessage)
	{

	}

	//! Constructor
	CMessage(const wchar_t * szMessage) :
		std::exception(vr::str::narrow(szMessage, std::locale()).c_str())
	{

	}

	//! Constructor
	CMessage(const std::string& sMessage) :
		std::exception(sMessage.c_str())
	{

	}

	//! Constructor
	CMessage(const std::wstring& sMessage) :
		std::exception(vr::str::narrow(sMessage, std::locale()).c_str())
	{

	}

	//! Destructor
	virtual ~CMessage()
	{

	}
};


//! Macros for convenient throwing
#define THROW_EXCEPTION(message) throw clrn::CException(__FILE__, __LINE__, message);
#define THROW_MESSAGE(message) throw clrn::CMessage(message);

//! Macroses for convenient catching
#define TRY_EXCEPTIONS try
#define CATCH_PASS_XCPT(excpt) catch (excpt& e)\
	{\
	const std::string sExcptname = (#excpt);\
	const std::string sMessage = (boost::format("\nCatch pass [%s]\nMessage:\n{%s}") % sExcptname % e.what()).str();\
	THROW_EXCEPTION(sMessage);\
	}

#define CATCH_PASS_XCPT_LOG(excpt, Log) catch (excpt& e)\
{\
	const std::string sExcptname = (#excpt);\
	const std::string sMessage = (boost::format("\nCatch pass [%s]\nMessage:\n{%s}") % sExcptname % e.what()).str();\
	(Log).Write(sMessage);\
	THROW_EXCEPTION(sMessage);\
}

#define CATCH_IGNORE_XCPT(excpt) catch (excpt& e)\
{\
	const std::string sExcptname = (#excpt);\
	const std::string sMessage = (boost::format("\nCatch ignore [%s]\nMessage:\n{%s}") % sExcptname % e.what()).str();\
}

#define CATCH_IGNORE_XCPT_LOG(excpt, Log) catch (excpt& e)\
{\
	const std::string sExcptname = (#excpt);\
	const std::string sMessage = (boost::format("\nCatch ignore [%s]\nMessage:\n{%s}") % sExcptname % e.what()).str();\
	(Log).Write(sMessage);\
}

#define CATCH_PASS_UNHANDLED_XCPT catch(...)\
{\
	const std::string sExcptname = "Unhandled exception";\
	const std::string sMessage = (boost::format("\nCatch pass [%s]\n") % sExcptname).str();\
	THROW_EXCEPTION(sMessage);\
}

#define CATCH_IGNORE_UNHANDLED_XCPT catch(...)\
{\
	const std::string sExcptname = "Unhandled exception";\
	const std::string sMessage = (boost::format("\nCatch ignore [%s]\n") % sExcptname).str();\
}

#define CATCH_PASS_UNHANDLED_XCPT_LOG(Log) catch(...)\
{\
	const std::string sExcptname = "Unhandled exception";\
	const std::string sMessage = (boost::format("\nCatch pass [%s]\n") % sExcptname).str();\
	(Log).Write(sMessage);\
	THROW_EXCEPTION(sMessage);\
}

#define CATCH_IGNORE_UNHANDLED_XCPT_LOG(Log) catch(...)\
{\
	const std::string sExcptname = "Unhandled exception";\
	const std::string sMessage = (boost::format("\nCatch pass [%s]\n") % sExcptname).str();\
	(Log).Write(sMessage);\
}

#define CATCH_THREAD_INTERRUPTED catch(boost::thread_interrupted&) {}

#define CATCH_THREAD_INTERRUPTED_LOG(Log) catch(boost::thread_interrupted&) {}

#define CATCH_PASS_USER_MESSAGE catch(clrn::CMessage& e)\
{\
	MessageBoxA(0, e.what(), "Error", MB_OK | MB_ICONERROR);\
	const std::string sExcptname = "User message";\
	const std::string sMessage = (boost::format("\nCatch pass [%s]\nMessage:\n{%s}") % sExcptname % e.what()).str();\
	THROW_EXCEPTION(sMessage);\
}

#define CATCH_IGNORE_USER_MESSAGE catch(clrn::CMessage& e)\
{\
	MessageBoxA(0, e.what(), "Error", MB_OK | MB_ICONERROR);\
}

#define CATCH_PASS_USER_MESSAGE_LOG(Log) catch(clrn::CMessage& e)\
{\
	MessageBoxA(0, e.what(), "Error", MB_OK | MB_ICONERROR);\
	const std::string sExcptname = "User message";\
	const std::string sMessage = (boost::format("\nCatch pass [%s]\nMessage:\n{%s}") % sExcptname % e.what()).str();\
	(Log).Write(sMessage);\
	THROW_EXCEPTION(sMessage);\
}

#define CATCH_IGNORE_USER_MESSAGE_LOG(Log) catch(clrn::CMessage& e)\
{\
	MessageBoxA(0, e.what(), "Error", MB_OK | MB_ICONERROR);\
	const std::string sExcptname = "User message";\
	const std::string sMessage = (boost::format("\nCatch pass [%s]\nMessage:\n{%s}") % sExcptname % e.what()).str();\
	(Log).Write(sMessage);\
}

#define CATCH_PASS_EXCEPTIONS\
	CATCH_THREAD_INTERRUPTED\
	CATCH_PASS_USER_MESSAGE\
	CATCH_PASS_XCPT(clrn::CException)\
	CATCH_PASS_XCPT(std::domain_error)\
	CATCH_PASS_XCPT(std::length_error)\
	CATCH_PASS_XCPT(std::overflow_error)\
	CATCH_PASS_XCPT(std::underflow_error)\
	CATCH_PASS_XCPT(std::range_error)\
	CATCH_PASS_XCPT(std::logic_error)\
	CATCH_PASS_XCPT(std::runtime_error)\
	CATCH_PASS_XCPT(std::exception)\
	CATCH_PASS_UNHANDLED_XCPT

#define CATCH_PASS_EXCEPTIONS_LOG(Log)\
	CATCH_THREAD_INTERRUPTED_LOG(Log)\
	CATCH_PASS_USER_MESSAGE_LOG(Log)\
	CATCH_PASS_XCPT_LOG(clrn::CException, Log)\
	CATCH_PASS_XCPT_LOG(std::overflow_error, Log)\
	CATCH_PASS_XCPT_LOG(std::underflow_error, Log)\
	CATCH_PASS_XCPT_LOG(std::range_error, Log)\
	CATCH_PASS_XCPT_LOG(std::domain_error, Log)\
	CATCH_PASS_XCPT_LOG(std::length_error, Log)\
	CATCH_PASS_XCPT_LOG(std::logic_error, Log)\
	CATCH_PASS_XCPT_LOG(std::runtime_error, Log)\
	CATCH_PASS_XCPT_LOG(std::exception, Log)\
	CATCH_PASS_UNHANDLED_XCPT_LOG(Log)

#define CATCH_IGNORE_EXCEPTIONS\
	CATCH_THREAD_INTERRUPTED\
	CATCH_IGNORE_USER_MESSAGE\
	CATCH_IGNORE_XCPT(clrn::CException)\
	CATCH_IGNORE_XCPT(std::domain_error)\
	CATCH_IGNORE_XCPT(std::length_error)\
	CATCH_IGNORE_XCPT(std::overflow_error)\
	CATCH_IGNORE_XCPT(std::underflow_error)\
	CATCH_IGNORE_XCPT(std::range_error)\
	CATCH_IGNORE_XCPT(std::logic_error)\
	CATCH_IGNORE_XCPT(std::runtime_error)\
	CATCH_IGNORE_XCPT(std::exception)\
	CATCH_IGNORE_UNHANDLED_XCPT

#define CATCH_IGNORE_EXCEPTIONS_LOG(Log)\
	CATCH_THREAD_INTERRUPTED_LOG(Log)\
	CATCH_IGNORE_USER_MESSAGE_LOG(Log)\
	CATCH_IGNORE_XCPT_LOG(clrn::CException, Log)\
	CATCH_IGNORE_XCPT_LOG(std::domain_error, Log)\
	CATCH_IGNORE_XCPT_LOG(std::length_error, Log)\
	CATCH_IGNORE_XCPT_LOG(std::overflow_error, Log)\
	CATCH_IGNORE_XCPT_LOG(std::underflow_error, Log)\
	CATCH_IGNORE_XCPT_LOG(std::range_error, Log)\
	CATCH_IGNORE_XCPT_LOG(std::logic_error, Log)\
	CATCH_IGNORE_XCPT_LOG(std::runtime_error, Log)\
	CATCH_IGNORE_XCPT_LOG(std::exception, Log)\
	CATCH_IGNORE_UNHANDLED_XCPT_LOG(Log)
}

#endif //_Exception_H_Included