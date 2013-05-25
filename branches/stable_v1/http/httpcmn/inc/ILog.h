/*
 * ILog.h
 *
 *  Created on: Nov 11, 2011
 *      Author: root
 */

#ifndef ILOG_H_
#define ILOG_H_

#include "Types.h"

namespace cmn
{

//! Boost::format wrapper
class CFmtWrap;

//! Interface of the logger
class ILog
{
private:

	//! Not implemented
	const ILog& operator = (const ILog&);
	ILog(const ILog&);

public:

	//! Levels
	enum class Level
	{
		eOff			= 0,
		eError			= 1,
		eMessage		= 2,
		eDebug			= 3
	};

	ILog() {}

	//! Logger initialization
	virtual void 					Open(const _tstring& sFileName, const Level) = 0;
	virtual void 					Open(_tostream& osStream, const Level) = 0;

	//! Is log enabled
	virtual bool					IsEnabled() = 0;

	//! Logger shutdown
	virtual void 					Shutdown() = 0;

	//! Write message
	virtual ILog&					operator << (const TCHAR * szMessage) = 0;
	virtual ILog&					operator << (const _tstring& sMessage) = 0;
	virtual ILog&					operator << (const CFmtWrap& fmtMessage) = 0;

	//! Write message
	virtual void 					Write(const _tstring& sMessage, const Level = Level::eMessage) = 0;
	virtual void 					Write(const TCHAR * szMessage, const Level = Level::eMessage) = 0;
	virtual void 					Write(const CFmtWrap& fmtMessage) = 0;
	virtual void 					Write(const TCHAR * szFile, const std::size_t& nLine, const _tstring& sMessage, const Level) = 0;
	virtual void 					Write(const TCHAR * szFile, const std::size_t& nLine, const TCHAR* szMessage, const Level) = 0;

	//! Virtual destructor
	virtual							~ILog() {}
};

}

#endif /* ILOG_H_ */
