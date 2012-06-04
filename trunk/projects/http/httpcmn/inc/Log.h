/*
 * Log.h
 *
 *  Created on: Nov 7, 2011
 *      Author: root
 */

#ifndef LOG_H_
#define LOG_H_

#include "ILog.h"

#include <boost/format.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/scoped_ptr.hpp>

namespace cmn
{

//! Boost::format wrapper
class CFmtWrap
{
public:

	CFmtWrap() = delete;
	CFmtWrap(const TCHAR * szMessage, ILog::Level eLevel = ILog::Level::eMessage) :
		m_fmtBoostFormat(szMessage),
		m_eLevel(eLevel)
	{

	}

	CFmtWrap(const _tstring& sMessage, ILog::Level eLevel = ILog::Level::eMessage) :
		m_fmtBoostFormat(sMessage),
		m_eLevel(eLevel)
	{

	}

	template < typename T >
	CFmtWrap& operator % (const T& Value)
	{
		m_fmtBoostFormat % Value;
		return *this;
	}

	void GetFormattedMessage (_tstring& sMessage, ILog::Level& eLevel) const
	{
		sMessage = m_fmtBoostFormat.str();
		eLevel = m_eLevel;
	}


private:

	//! boost format object
	boost::basic_format< TCHAR > 	m_fmtBoostFormat;

	//! Message level
	ILog::Level						m_eLevel;
};

class CLog : public ILog
{
public:

	//! Class for scoped logging
	class CScopedLog
	{
		typedef std::map< std::size_t, std::size_t > ThreadsMap;
	public:
		CScopedLog(ILog& Logger, const char * szFunctionName);
		~CScopedLog();
	private:
		ILog&							m_Log;
		const TCHAR *					m_szFunction;
		std::size_t						m_nThreadId;
		static ThreadsMap				ms_mapThreads;
		static boost::recursive_mutex	m_mxMap;
	};

	CLog();

	//! Logger initialization
	virtual void 					Open(const _tstring& sFileName, const Level);
	virtual void 					Open(_tostream& osStream, const Level);

	//! Logger shutdown
	virtual void 					Shutdown();

	//! Is log enabled
	inline virtual bool				IsEnabled(){ return m_eLevel > Level::eOff;}

	virtual ILog&					operator << (const TCHAR * szMessage);
	virtual ILog&					operator << (const _tstring& sMessage);
	virtual ILog&					operator << (const CFmtWrap& fmtMessage);

	//! Write methods
	virtual void 					Write(const _tstring& sMessage, const Level = Level::eMessage);
	virtual void 					Write(const TCHAR * szMessage, const Level = Level::eMessage);
	virtual void 					Write(const CFmtWrap& fmtMessage);
	virtual void 					Write(const TCHAR * szFile, const std::size_t& nLine, const _tstring& sMessage, const Level);
	virtual void 					Write(const TCHAR * szFile, const std::size_t& nLine, const TCHAR* szMessage, const Level);

	virtual ~CLog();

private:

	//! Write string prefix
	void 							WritePrefix();

	//! Write string postfix
	void 							WritePostfix();

	//! Is stream external
	bool 							m_bIsStreamExternal;

	//! Pointer to logger stream
	_tostream * 					m_pStream;

	//! Log level
	Level							m_eLevel;

	//! Mutex for log operations
	boost::recursive_mutex			m_mxLogger;
};

} /* namespace cmn */
#endif /* LOG_H_ */
