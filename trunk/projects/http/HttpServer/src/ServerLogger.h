/*
 * ServerLogger.h
 *
 *  Created on: Nov 11, 2011
 *      Author: root
 */

#ifndef SERVERLOGGER_H_
#define SERVERLOGGER_H_

#include "Log.h"

#include <cassert>

#include <boost/current_function.hpp>

namespace cmn
{

class Logger
{
public:
	static inline ILog& Instance()
	{
		assert(ms_pLogger);
		return *ms_pLogger;
	}

	static inline void Init(ILog * pLogger)
	{
		assert(pLogger);

		ms_pLogger = pLogger;
	}

private:

	static ILog * ms_pLogger;
};

//! Macro for logging functions
#define SCOPED_LOG_FUNCTION_LOG(log) cmn::CLog::CScopedLog __ScopedLog(log, BOOST_CURRENT_FUNCTION);
#define SCOPED_LOG_FUNCTION SCOPED_LOG_FUNCTION_LOG(cmn::Logger::Instance())

}

#endif /* SERVERLOGGER_H_ */
