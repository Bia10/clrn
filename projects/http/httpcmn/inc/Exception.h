/*
 * Exception.h
 *
 *  Created on: Nov 7, 2011
 *      Author: root
 */

/*
 * Exception.hpp
 *
 *  Created on: Nov 7, 2011
 *      Author: root
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include "Status.h"

//! Include STL library headers
#include <stdexcept>
#include <string>
#include <sstream>

namespace cmn{

//! Exception implementation
class CException : public std::exception
{
public:
	//! Constructors
	CException() = delete;
	CException(const char * szFile, const std::size_t& nLine, const CStatus::StatusCode& ecCode, const char * szMessage);
	CException(const char * szFile, const std::size_t& nLine, const CStatus::StatusCode& ecCode, const wchar_t * szMessage);
	CException(const char * szFile, const std::size_t& nLine, const CStatus::StatusCode& ecCode, const std::string& sMessage);
	CException(const char * szFile, const std::size_t& nLine, const CStatus::StatusCode& ecCode, const std::wstring& sMessage);

	//! Destructor
	virtual 					~CException() throw();

	//! Overrided method to get description of the error
	virtual const char * 		what() const throw();

	//! Get error code
	const CStatus::StatusCode&	code() const throw();

private:

	//! Format error message
	const std::string 			Format(const char * szFile, const std::size_t& nLine, const CStatus::StatusCode& ecCode, const char * szMessage) const;
	const std::string 			Format(const char * szFile, const std::size_t& nLine, const CStatus::StatusCode& ecCode, const wchar_t * szMessage) const;

	//! Message string
	std::string					m_sMessage;

	//! Error code
	const CStatus::StatusCode	m_ecError;
};

} // namespace cmn

//! Macros for convenient throwing
#define THROW_EXCEPTION_CODE(code, message) throw cmn::CException(__FILE__, __LINE__, static_cast< cmn::CStatus::StatusCode > (code), message);
#define THROW_EXCEPTION(message) throw cmn::CException(__FILE__, __LINE__, cmn::CStatus::StatusCode::eInternalServerError, message);

//! Macroses for convenient catching
#define TRY_EXCEPTIONS try

#define CATCH_PASS_CEXCEPTION catch (cmn::CException& e)\
{\
	std::ostringstream ossMessage;\
	ossMessage \
		<< std::endl << "Passed [" << "cmn::CException" << "]" << std::endl\
		<< "nMessage:{" << std::endl << e.what() << "}";\
	THROW_EXCEPTION_CODE(e.code(), ossMessage.str());\
}

#define CATCH_PASS_CEXCEPTION_LOG(Log) catch (cmn::CException& e)\
{\
	std::ostringstream ossMessage;\
	ossMessage \
		<< std::endl << "Passed [" << "cmn::CException" << "]" << std::endl\
		<< "nMessage:{" << std::endl << e.what() << "}";\
	(Log).Write(ossMessage.str(), cmn::ILog::Level::eError);\
	THROW_EXCEPTION_CODE(e.code(), ossMessage.str());\
}

#define CATCH_PASS_XCPT(excpt) catch (excpt& e)\
{\
	std::ostringstream ossMessage;\
	ossMessage \
		<< std::endl << "Passed [" << (#excpt) << "]" << std::endl\
		<< "nMessage:{" << std::endl << e.what() << "}";\
	THROW_EXCEPTION(ossMessage.str());\
}

#define CATCH_PASS_XCPT_LOG(excpt, Log) catch (excpt& e)\
{\
	std::ostringstream ossMessage;\
	ossMessage \
		<< std::endl << "Passed [" << (#excpt) << "]" << std::endl\
		<< "nMessage:{" << std::endl << e.what() << "}";\
	(Log).Write(ossMessage.str(), cmn::ILog::Level::eError);\
	THROW_EXCEPTION(ossMessage.str());\
}

#define CATCH_IGNORE_XCPT(excpt) catch (excpt& e)\
{\
}

#define CATCH_IGNORE_XCPT_LOG(excpt, Log) catch (excpt& e)\
{\
	std::ostringstream ossMessage;\
	ossMessage \
		<< std::endl << "Ignored [" << (#excpt) << "]" << std::endl\
		<< "nMessage:{" << std::endl << e.what() << "}";\
	(Log).Write(ossMessage.str(), cmn::ILog::Level::eError);\
}

#define CATCH_PASS_UNHANDLED_XCPT catch(...)\
{\
	std::ostringstream ossMessage;\
	ossMessage \
		<< std::endl << "Passed [" << "Unhandled exception" << "]" << std::endl;\
	THROW_EXCEPTION(ossMessage.str());\
}

#define CATCH_IGNORE_UNHANDLED_XCPT catch(...)\
{\
}

#define CATCH_PASS_UNHANDLED_XCPT_LOG(Log) catch(...)\
{\
	std::ostringstream ossMessage;\
	ossMessage \
		<< std::endl << "Passed [" << "Unhandled exception" << "]" << std::endl;\
	(Log).Write(ossMessage.str(), cmn::ILog::Level::eError);\
	THROW_EXCEPTION(ossMessage.str());\
}

#define CATCH_IGNORE_UNHANDLED_XCPT_LOG(Log) catch(...)\
{\
	std::ostringstream ossMessage;\
	ossMessage \
		<< std::endl << "Passed [" << "Unhandled exception" << "]" << std::endl;\
	(Log).Write(ossMessage.str(), cmn::ILog::Level::eError);\
}

#define CATCH_PASS_EXCEPTIONS\
	CATCH_PASS_CEXCEPTION\
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
	CATCH_PASS_CEXCEPTION_LOG(Log)\
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
	CATCH_IGNORE_XCPT(cmn::CException)\
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
	CATCH_IGNORE_XCPT_LOG(cmn::CException, Log)\
	CATCH_IGNORE_XCPT_LOG(std::domain_error, Log)\
	CATCH_IGNORE_XCPT_LOG(std::length_error, Log)\
	CATCH_IGNORE_XCPT_LOG(std::overflow_error, Log)\
	CATCH_IGNORE_XCPT_LOG(std::underflow_error, Log)\
	CATCH_IGNORE_XCPT_LOG(std::range_error, Log)\
	CATCH_IGNORE_XCPT_LOG(std::logic_error, Log)\
	CATCH_IGNORE_XCPT_LOG(std::runtime_error, Log)\
	CATCH_IGNORE_XCPT_LOG(std::exception, Log)\
	CATCH_IGNORE_UNHANDLED_XCPT_LOG(Log)



#endif /* EXCEPTION_H_ */
