#include "stdafx.h"
#include "Log.h"
#include "Conversion.h"
#include "FileSystem.h"

#ifdef WIN32
#include <windows.h>
#endif

//! Class for scoped logging
class CScopedLog : boost::noncopyable
{
public:
	CScopedLog(ILog& Logger, const char *szFunctionName) 
		: m_Log(Logger)
		, m_szFunction(szFunctionName)
#ifdef WIN32
		, m_nThreadId(GetCurrentThreadId())
#else
		, m_nThreadId(pthread_self())
#endif
	{
		std::wstring indent;
		{
			boost::mutex::scoped_lock Lock(s_mxMap);
			indent.assign(++s_mapThreads[m_nThreadId], L'-');
		}

		m_Log.Debug(L" -%s> [%s]") % indent % m_szFunction;
	}

	~CScopedLog()
	{
		std::wstring indent;
		{
			boost::mutex::scoped_lock Lock(s_mxMap);
			indent.assign(s_mapThreads[m_nThreadId]--, L'-');
		}

		m_Log.Debug(L" <-%s [%s]") % indent % m_szFunction;
	}
private:

	typedef std::map<std::size_t, std::size_t> ThreadsMap;
	ILog&					m_Log;
	const std::string		m_szFunction;
	std::size_t				m_nThreadId;
	static ThreadsMap		s_mapThreads;
	static boost::mutex		s_mxMap;
};

CScopedLog::ThreadsMap 	CScopedLog::s_mapThreads;
boost::mutex		 	CScopedLog::s_mxMap;

CLog::CLog(void)
	: m_pStream(0)
	, m_IsExternalStream(false)
	, m_CurrentLevel(Level::None)
	, m_IsOpened(false)
{
}

CLog::~CLog(void)
{
	Close();
}

void CLog::Open(const std::string& source)
{
	Open(conv::cast<std::wstring>(source));
}

void CLog::Open(const std::wstring& source)
{
	if (1 == source.size())
	{
		switch(*source.begin())
		{
		case '1' :
			m_pStream = &std::cout;
			break;
		case '2' :
			m_pStream = &std::cerr;
			break;
		case '3' :
			m_pStream = &std::clog;
			break;
		}
		m_IsExternalStream = true;
	}
	else
	{
		if (fs::Exists(source))
		{
			// file exists, need to save it first
			std::wstring newPath = fs::FullPath(source);

			std::wstring::size_type dot = newPath.rfind('.');
			if (std::wstring::npos == dot)
				dot = newPath.size() - 1;

			newPath.insert(dot, boost::posix_time::to_iso_wstring(boost::posix_time::microsec_clock().local_time()));

			fs::Move(fs::FullPath(source), newPath);
		}

		std::ofstream* stream = new std::ofstream(source.c_str(), std::ios::out);
		CHECK(stream->is_open(), conv::cast<std::string>(source));
		m_pStream = stream;
		m_IsExternalStream = false;
	}	


	m_Mutex.lock();
	m_Format = boost::format("Logger started.");
	Write(Level::None);
	m_IsOpened = true;
}

void CLog::Close()
{
	if (!m_IsOpened)
		return;

	m_IsOpened = false;

	m_Mutex.lock();
	m_Format = boost::format("Logger shutted down.");
	Write(Level::None);

	if (m_IsExternalStream)
		return;

	delete m_pStream;
	m_pStream = 0;
}

bool CLog::IsEnabled(unsigned int module, const Level::Enum_t level) const
{
	if (m_Levels.size() <= module)
		return false;

	return m_Levels[module] >= level;
}

ILog& CLog::Error(const std::string& text)
{
	m_Mutex.lock();
	m_Format = boost::format(text);
	m_CurrentLevel = Level::Error;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Error(const std::wstring& text)
{
	return Error(conv::cast<std::string>(text).c_str());
}

ILog& CLog::Error(const std::string& func, const std::string& text)
{
	m_Mutex.lock();
	m_Format = boost::format(std::string(text) + " /* " + func + " */");
	m_CurrentLevel = Level::Error;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Error(const std::string& func, const std::wstring& text)
{
	return Error(func, conv::cast<std::string>(text).c_str());
}

ILog& CLog::Warning(const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(text);
	m_CurrentLevel = Level::Warning;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Warning(const std::wstring& text)
{
	return Warning(conv::cast<std::string>(text).c_str());
}

ILog& CLog::Warning(const std::string& func, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(std::string(text) + " /* " + func + " */");
	m_CurrentLevel = Level::Warning;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Warning(const std::string& func, const std::wstring& text)
{
	return Warning(func, conv::cast<std::string>(text).c_str());
}

ILog& CLog::Trace(const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(text);
	m_CurrentLevel = Level::Trace;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Trace(const std::wstring& text)
{
	return Trace(conv::cast<std::string>(text).c_str());
}

ILog& CLog::Trace(const std::string& func, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(std::string(text) + " /* " + func + " */");
	m_CurrentLevel = Level::Trace;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Trace(const std::string& func, const std::wstring& text)
{
	return Trace(func, conv::cast<std::string>(text).c_str());
}

ILog& CLog::Debug(const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(text);
	m_CurrentLevel = Level::Debug;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Debug(const std::wstring& text)
{
	return Debug(conv::cast<std::string>(text).c_str());
}

ILog& CLog::Debug(const std::string& func, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(std::string(text) + " /* " + func + " */");
	m_CurrentLevel = Level::Debug;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Debug(const std::string& func, const std::wstring& text)
{
	return Debug(func, conv::cast<std::string>(text).c_str());
}

ILog& CLog::operator%(const unsigned int value)
{
	if (!m_Format.remaining_args())
		return *this;

	m_Format.operator %(value);

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::operator%(const int value)
{
	if (!m_Format.remaining_args())
		return *this;

	m_Format.operator %(value);

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::operator%(const double value)
{
	if (!m_Format.remaining_args())
		return *this;

	m_Format.operator %(value);

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::operator%(const unsigned long value)
{
	if (!m_Format.remaining_args())
		return *this;

	m_Format.operator %(value);

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::operator%(const long value)
{
	if (!m_Format.remaining_args())
		return *this;

	m_Format.operator %(value);

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::operator%(const std::wstring& value)
{
	return operator % (conv::cast<std::string>(value));
}

ILog& CLog::operator%(const std::string& value)
{
	if (!m_Format.remaining_args())
		return *this;

	m_Format.operator %(value);

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

const char* CLog::Level2String(const Level::Enum_t level) const
{
	switch(level)
	{
	case Level::None:
		return "NONE   ";
	case Level::Error:
		return "ERROR  ";
	case Level::Warning:
		return "WARNING";
	case Level::Trace:
		return "TRACE  ";
	case Level::Debug:
		return "DEBUG  ";
	}
	return "UNKNOWN";
}

void CLog::Write(const Level::Enum_t level)
{
	try
	{
		if (!m_pStream)
			return;

		*m_pStream
			<< "[" << Level2String(level) << "]"
			<< "[" << boost::posix_time::to_iso_extended_string(boost::posix_time::microsec_clock().local_time()) << "]:["
#ifdef WIN32
			<< conv::cast<std::string>(GetCurrentThreadId())
#else
			<< conv::cast<std::string>(pthread_self())
#endif
			<< "]: " << m_Format.str() << std::endl;

	}
	catch(...)
	{

	}

	m_Mutex.unlock();
}

ILog::ScopedLogPtr CLog::MakeScopedLog(unsigned int module, const std::string& func)
{
	if (!IsEnabled(module, Level::Debug))
		return ScopedLogPtr();

	return ScopedLogPtr(new CScopedLog(*this, func.c_str()));
}

void CLog::SetLogLevels(const std::vector<Level::Enum_t>& levels)
{
	m_Levels = levels;

	for (unsigned int i = 0 ; i < m_Levels.size(); ++i)
		Warning(__FUNCTION__, "Setting up module: [%s] log level: [%s].") % i % m_Levels[i];
}