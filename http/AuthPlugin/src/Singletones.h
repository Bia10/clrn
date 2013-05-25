/*
 * Singletones.h
 *
 *  Created on: Nov 16, 2011
 *      Author: root
 */

#ifndef SINGLETONES_H_
#define SINGLETONES_H_

#include "ILog.h"
#include "ISettings.h"

#include <cassert>

//! Singltone for logger
class Logger
{
public:
	inline static cmn::ILog& Instance()
	{
		assert(ms_pLogger);

		return *ms_pLogger;
	}
	inline static void Init(cmn::ILog& Logger)
	{
		ms_pLogger = &Logger;
	}

private:
	static cmn::ILog * ms_pLogger;
};

//! Singltone for settings
class Settings
{
public:
	inline static cmn::ISettings& Instance()
	{
		assert(ms_pSettings);

		return *ms_pSettings;
	}
	inline static void Init(cmn::ISettings& Settings)
	{
		ms_pSettings = &Settings;
	}

private:
	static cmn::ISettings * ms_pSettings;
};

#endif /* SINGLETONES_H_ */
