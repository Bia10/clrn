/*
 * Log.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: root
 */

#include "Log.h"

#include <fstream>
#include <algorithm>
#include <cassert>

#ifdef WIN32
#include <windows.h>
#endif

#include <boost/format.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>

namespace cmn
{

CLog::CLog() :
		m_bIsStreamExternal(false),
		m_pStream(0),
		m_eLevel(Level::eOff)
{

}

CLog::~CLog()
{
	Shutdown();
}

void CLog::Open(const _tstring & sFileName, const Level level)
{
	m_pStream = new std::basic_ofstream< TCHAR > (sFileName.c_str(), std::ios::out);
	m_eLevel = level;
	m_bIsStreamExternal = false;

	if (IsEnabled())
	{
		WritePrefix();
		*m_pStream << _T("Logger started.");
		WritePostfix();
	}
}

void CLog::Open(_tostream & osStream, const Level level)
{
	m_pStream = &osStream;
	m_eLevel = level;
	m_bIsStreamExternal = true;

	if (IsEnabled())
	{
		WritePrefix();
		*m_pStream << _T("Logger started.");
		WritePostfix();
	}
}

void CLog::Shutdown()
{
	if (!m_pStream)
		return;

	m_pStream->flush();
	if (!m_bIsStreamExternal)
		delete m_pStream;
}

ILog & CLog::operator <<(const TCHAR *szMessage)
{
	Write(szMessage);
	return *this;
}

ILog & CLog::operator <<(const _tstring & sMessage)
{
	Write(sMessage);
	return *this;
}

void CLog::Write(const _tstring & sMessage, const Level level)
{
	Write(sMessage.c_str(), level);
}

void CLog::Write(const TCHAR *szMessage, const Level level)
{
	boost::recursive_mutex::scoped_lock Lock(m_mxLogger);

	if (level > m_eLevel)
		return;

	WritePrefix();
	*m_pStream << szMessage;
	WritePostfix();

}

void CLog::Write(const TCHAR *szFile, const std::size_t & nLine, const _tstring & sMessage, const Level level)
{
	Write(szFile, nLine, sMessage, level);
}

void CLog::Write(const TCHAR *szFile, const std::size_t & nLine, const TCHAR *szMessage, const Level level)
{
	boost::recursive_mutex::scoped_lock Lock(m_mxLogger);

	if (level > m_eLevel)
		return;

	WritePrefix();
	*m_pStream << _T("File : ") << szFile << _T( " Line: ") << nLine << _T(" Message: ") << szMessage;
	WritePostfix();
}

void CLog::WritePrefix()
{
	assert(m_pStream);

	*m_pStream
		<< _T("[") << boost::posix_time::to_iso_extended_string(boost::posix_time::microsec_clock().local_time()) << _T("]:[")
#ifdef WIN32
		<< boost::lexical_cast< _tstring > (GetCurrentThreadId())
#else
		<< boost::lexical_cast< _tstring > (pthread_self())
#endif
		<< _T("]: {");
}

ILog & CLog::operator <<(const CFmtWrap & fmtMessage)
{
	Write(fmtMessage);

	return *this;
}

void CLog::Write(const CFmtWrap & fmtMessage)
{
	_tstring sTemp;
	ILog::Level eTemp;
	fmtMessage.GetFormattedMessage(sTemp, eTemp);
	Write(sTemp, eTemp);
}

void CLog::WritePostfix()
{
	assert(m_pStream);

	*m_pStream << _T(" } ") << std::endl;
}

CLog::CScopedLog::ThreadsMap 	CLog::CScopedLog::ms_mapThreads;
boost::recursive_mutex 			CLog::CScopedLog::m_mxMap;

CLog::CScopedLog::CScopedLog(ILog & Logger, const char *szFunctionName) :
		m_Log(Logger)
		, m_szFunction(szFunctionName)
#ifdef WIN32
		, m_nThreadId(GetCurrentThreadId())
#else
		, m_nThreadId(pthread_self())
#endif

{
	if (m_Log.IsEnabled())
	{
		_tstring sIndent;
		{
			boost::recursive_mutex::scoped_lock Lock(m_mxMap);
			sIndent.assign(++ms_mapThreads[m_nThreadId], '-');
		}

		m_Log.Write(CFmtWrap(_T(" -%s> [%s]"), ILog::Level::eDebug) % sIndent % m_szFunction);
	}
}

CLog::CScopedLog::~CScopedLog()
{
	if (m_Log.IsEnabled())
	{
		_tstring sIndent;
		{
			boost::recursive_mutex::scoped_lock Lock(m_mxMap);
			sIndent.assign(ms_mapThreads[m_nThreadId]--, '-');
		}

		m_Log.Write(CFmtWrap(_T(" <-%s [%s]"), ILog::Level::eDebug) % sIndent % m_szFunction);
	}
}


}

 /* namespace cmn */
