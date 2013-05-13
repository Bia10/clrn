#include "Log.h"
#include "Conversion.h"
#include "FileSystem.h"

#include <fstream>

#ifdef WIN32
#include <windows.h>
#endif

//! Class for scoped logging
class CScopedLog : boost::noncopyable
{
public:
	CScopedLog(ILog& Logger, const char *szFunctionName, unsigned int module) 
		: m_Log(Logger)
		, m_szFunction(szFunctionName)
#ifdef WIN32
		, m_nThreadId(GetCurrentThreadId())
#else
		, m_nThreadId(pthread_self())
#endif
		, m_Module(module)
	{
		std::wstring indent;
		{
			boost::mutex::scoped_lock Lock(s_mxMap);
			indent.assign(++s_mapThreads[m_nThreadId], L'-');
		}

		m_Log.Debug(m_Module, L" -%s> [%s]") % indent % m_szFunction;
	}

	~CScopedLog()
	{
		std::wstring indent;
		{
			boost::mutex::scoped_lock Lock(s_mxMap);
			indent.assign(s_mapThreads[m_nThreadId]--, L'-');
		}

		m_Log.Debug(m_Module, L" <-%s [%s]") % indent % m_szFunction;
	}
private:

	typedef std::map<std::size_t, std::size_t> ThreadsMap;
	ILog&					m_Log;
	const std::string		m_szFunction;
	std::size_t				m_nThreadId;
	static ThreadsMap		s_mapThreads;
	static boost::mutex		s_mxMap;
	unsigned int			m_Module;
};

CScopedLog::ThreadsMap 	CScopedLog::s_mapThreads;
boost::mutex		 	CScopedLog::s_mxMap;

Log::Log(void)
	: m_CurrentLevel(Level::None)
	, m_IsOpened(false)
	, m_CurrentModule(0)
{
}

Log::~Log(void)
{
	Close();
}

void Log::Open(const std::string& source, unsigned int module, const Level::Enum_t level)
{
	Open(conv::cast<std::wstring>(source), module, level);
}

void Log::Open(const std::wstring& source, unsigned int module, const Level::Enum_t level)
{
	if (m_Streams.size() <= module)
		m_Streams.resize(module + 1);

	if (!m_Streams[module])
		m_Streams[module].reset(new log_details::LogStream());

	if (1 == source.size())
	{
		switch(*source.begin())
		{
		case '1' :
			m_Streams[module]->AddStream(&std::cout, level, false);
			break;
		case '2' :
			m_Streams[module]->AddStream(&std::cerr, level, false);
			break;
		case '3' :
			m_Streams[module]->AddStream(&std::clog, level, false);
			break;
		}
	}
	else
	{
		if (m_StreamIndexes.count(source))
		{
			// this stream already opened
			m_Streams[module]->AddStream(m_StreamIndexes[source], level, false);
		}
		else
		{

			if (fs::Exists(source))
			{
				// file exists, need to save it first
				std::wstring newPath = fs::GetDirectory(fs::FullPath(source));

				std::wstring time = std::wstring(L"].") + boost::posix_time::to_iso_extended_wstring(boost::posix_time::microsec_clock().local_time());
				boost::algorithm::replace_all(time, L":", L".");
				boost::algorithm::replace_all(time, L"T", L"_");

				newPath += L"\\[";
				newPath +=  fs::GetFileName(source);
				newPath += time;
				newPath += fs::GetExtension(source);

				fs::Move(fs::FullPath(source), newPath);
			}

			const std::wstring fullPath = fs::FullPath(source);

			fs::CreateDirectories(fullPath);
			std::auto_ptr<std::ofstream> stream(new std::ofstream(fullPath.c_str(), std::ios::out));
			CHECK(stream->is_open(), conv::cast<std::string>(source));
			m_Streams[module]->AddStream(stream.release(), level, true);
		}
	}	


	m_Mutex.lock();
	m_CurrentModule = module;
	m_Format = boost::format("Logger started, module: [%s], level: [%s], source: [%s].") % module % level % conv::cast<std::string>(source);
	Write(Level::None);
	m_IsOpened = true;
}

void Log::Close()
{
	if (!m_IsOpened)
		return;

	m_IsOpened = false;

	m_Mutex.lock();
	m_Format = boost::format("Logger stopped.");
	Write(Level::None);

	m_Streams.clear();
}

bool Log::IsEnabled(unsigned int module, const Level::Enum_t level) const
{
	if (module >= m_Streams.size() || !m_Streams[module])
		return false;
	return m_Streams[module]->IsEnabled(level);
}

ILog& Log::Error(unsigned int module, const std::string& text)
{
	m_Mutex.lock();
	m_Format = boost::format(text);
	m_CurrentLevel = Level::Error;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& Log::Error(unsigned int module, const std::wstring& text)
{
	return Error(module, conv::cast<std::string>(text).c_str());
}

ILog& Log::Error(unsigned int module, const std::string& func, const std::string& text)
{
	m_Mutex.lock();
	m_Format = boost::format(std::string(text) + " /* " + func + " */");
	m_CurrentLevel = Level::Error;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& Log::Error(unsigned int module, const std::string& func, const std::wstring& text)
{
	return Error(module, func, conv::cast<std::string>(text).c_str());
}

ILog& Log::Warning(unsigned int module, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(text);
	m_CurrentLevel = Level::Warning;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& Log::Warning(unsigned int module, const std::wstring& text)
{
	return Warning(module, conv::cast<std::string>(text).c_str());
}

ILog& Log::Warning(unsigned int module, const std::string& func, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(text + " /* " + func + " */");
	m_CurrentLevel = Level::Warning;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& Log::Warning(unsigned int module, const std::string& func, const std::wstring& text)
{
	return Warning(module, func, conv::cast<std::string>(text).c_str());
}

ILog& Log::Trace(unsigned int module, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(text);
	m_CurrentLevel = Level::Trace;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& Log::Trace(unsigned int module, const std::wstring& text)
{
	return Trace(module, conv::cast<std::string>(text).c_str());
}

ILog& Log::Trace(unsigned int module, const std::string& func, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(std::string(text) + " /* " + func + " */");
	m_CurrentLevel = Level::Trace;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& Log::Trace(unsigned int module, const std::string& func, const std::wstring& text)
{
	return Trace(module, func, conv::cast<std::string>(text).c_str());
}

ILog& Log::Debug(unsigned int module, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(text);
	m_CurrentLevel = Level::Debug;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& Log::Debug(unsigned int module, const std::wstring& text)
{
	return Debug(module, conv::cast<std::string>(text).c_str());
}

ILog& Log::Debug(unsigned int module, const std::string& func, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(std::string(text) + " /* " + func + " */");
	m_CurrentLevel = Level::Debug;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& Log::Debug(unsigned int module, const std::string& func, const std::wstring& text)
{
	return Debug(module, func, conv::cast<std::string>(text).c_str());
}

ILog& Log::operator%(const LogAnyHolder& value)
{
    if (!m_Format.remaining_args())
        return *this;

    m_Format.operator % (value.operator std::string());

    if (!m_Format.remaining_args())
        Write(m_CurrentLevel);

    return *this;
}

const char* Log::Level2String(const Level::Enum_t level) const
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

void Log::Write(const Level::Enum_t level)
{
	try
	{
		if (m_Streams.size() <= m_CurrentModule || !m_Streams[m_CurrentModule])
		{
			m_Mutex.unlock();
			return;
		}

		m_TempStream.str("");

		m_TempStream
			<< "[" << Level2String(level) << "][" << m_CurrentModule
			<< "][" << boost::posix_time::to_iso_extended_string(boost::posix_time::microsec_clock().local_time()) << "]:["
#ifdef WIN32
			<< conv::cast<std::string>(GetCurrentThreadId())
#else
			<< conv::cast<std::string>(pthread_self())
#endif
			<< "]: " << m_Format.str() << std::endl;

		m_Streams[m_CurrentModule]->Write(m_TempStream.str(), level);
	}
	catch(...)
	{

	}

	m_Mutex.unlock();
}

ILog::ScopedLogPtr Log::MakeScopedLog(unsigned int module, const std::string& func)
{
	if (!IsEnabled(module, Level::Debug))
		return ScopedLogPtr();

	return ScopedLogPtr(new CScopedLog(*this, func.c_str(), module));
}

