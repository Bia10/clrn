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

CLog::CLog(void)
	: m_CurrentLevel(Level::None)
	, m_IsOpened(false)
	, m_CurrentModule(0)
{
}

CLog::~CLog(void)
{
	Close();
}

void CLog::Open(const std::string& source, unsigned int module)
{
	Open(conv::cast<std::wstring>(source), module);
}

void CLog::Open(const std::wstring& source, unsigned int module)
{
	if (m_Streams.size() <= module)
	{
		m_Streams.resize(module + 1);
		m_CreatedStreams.resize(m_Streams.size());
	}

	if (1 == source.size())
	{
		switch(*source.begin())
		{
		case '1' :
			m_Streams[module] = &std::cout;
			break;
		case '2' :
			m_Streams[module] = &std::cerr;
			break;
		case '3' :
			m_Streams[module] = &std::clog;
			break;
		}
		m_CreatedStreams[module] = false;
	}
	else
	{
		if (m_StreamIndexes.count(source))
		{
			// this stream already opened
			m_Streams[module] = m_Streams[m_StreamIndexes[source]];
			m_CreatedStreams[module] = false;
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

			fs::CreateDirectories(source);
			std::ofstream* stream = new std::ofstream(fs::FullPath(source).c_str(), std::ios::out);
			CHECK(stream->is_open(), conv::cast<std::string>(source));
			m_Streams[module] = stream;
			m_CreatedStreams[module] = true;
			m_StreamIndexes[source] = module;
		}
	}	


	m_Mutex.lock();
	m_CurrentModule = module;
	m_Format = boost::format("Logger started, module: [%s], source: [%s].") % module % conv::cast<std::string>(source);
	Write(Level::None);
	m_IsOpened = true;
}

void CLog::Close()
{
	if (!m_IsOpened)
		return;

	m_Packet.reset();
	m_IsOpened = false;

	m_Mutex.lock();
	m_Format = boost::format("Logger stopped.");
	Write(Level::None);

	for (std::size_t i = 0 ; i < m_Streams.size(); ++i)
	{
		if (m_CreatedStreams[i])
			delete m_Streams[i];
		m_CreatedStreams[i] = false;
	}
}

bool CLog::IsEnabled(unsigned int module, const Level::Enum_t level) const
{
	if (m_Levels.size() <= module)
		return false;

	return m_Levels[module] >= level;
}

ILog& CLog::Error(unsigned int module, const std::string& text)
{
	m_Mutex.lock();
	m_Format = boost::format(text);
	m_CurrentLevel = Level::Error;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Error(unsigned int module, const std::wstring& text)
{
	return Error(module, conv::cast<std::string>(text).c_str());
}

ILog& CLog::Error(unsigned int module, const std::string& func, const std::string& text)
{
	m_Mutex.lock();
	m_Format = boost::format(std::string(text) + " /* " + func + " */");
	m_CurrentLevel = Level::Error;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Error(unsigned int module, const std::string& func, const std::wstring& text)
{
	return Error(module, func, conv::cast<std::string>(text).c_str());
}

ILog& CLog::Warning(unsigned int module, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(text);
	m_CurrentLevel = Level::Warning;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Warning(unsigned int module, const std::wstring& text)
{
	return Warning(module, conv::cast<std::string>(text).c_str());
}

ILog& CLog::Warning(unsigned int module, const std::string& func, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(text + " /* " + func + " */");
	m_CurrentLevel = Level::Warning;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Warning(unsigned int module, const std::string& func, const std::wstring& text)
{
	return Warning(module, func, conv::cast<std::string>(text).c_str());
}

ILog& CLog::Trace(unsigned int module, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(text);
	m_CurrentLevel = Level::Trace;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Trace(unsigned int module, const std::wstring& text)
{
	return Trace(module, conv::cast<std::string>(text).c_str());
}

ILog& CLog::Trace(unsigned int module, const std::string& func, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(std::string(text) + " /* " + func + " */");
	m_CurrentLevel = Level::Trace;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Trace(unsigned int module, const std::string& func, const std::wstring& text)
{
	return Trace(module, func, conv::cast<std::string>(text).c_str());
}

ILog& CLog::Debug(unsigned int module, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(text);
	m_CurrentLevel = Level::Debug;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Debug(unsigned int module, const std::wstring& text)
{
	return Debug(module, conv::cast<std::string>(text).c_str());
}

ILog& CLog::Debug(unsigned int module, const std::string& func, const std::string& text)
{
	m_Mutex.lock();

	m_Format = boost::format(std::string(text) + " /* " + func + " */");
	m_CurrentLevel = Level::Debug;
	m_CurrentModule = module;

	if (!m_Format.remaining_args())
		Write(m_CurrentLevel);

	return *this;
}

ILog& CLog::Debug(unsigned int module, const std::string& func, const std::wstring& text)
{
	return Debug(module, func, conv::cast<std::string>(text).c_str());
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

boost::mutex g_PacketMutex;

void CLog::Write(const Level::Enum_t level)
{
	try
	{
		if (!m_Streams[m_CurrentModule])
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

		*m_Streams[m_CurrentModule] << m_TempStream.str();

		boost::mutex::scoped_lock lock(g_PacketMutex);
		if (m_Packet)
		{
			m_TempStream << "\t";
			m_Packet->add_trace(m_TempStream.str());
		}
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

	return ScopedLogPtr(new CScopedLog(*this, func.c_str(), module));
}

void CLog::SetLogLevels(const std::vector<Level::Enum_t>& levels)
{
	m_Levels = levels;

	m_Streams.resize(m_Levels.size());
	m_CreatedStreams.resize(m_Levels.size());

	for (unsigned int i = 0 ; i < m_Levels.size(); ++i)
		Warning(i, __FUNCTION__, "Setting up module: [%s] log level: [%s].") % i % m_Levels[i];
}


ILog& CLog::operator << (const ProtoPacketPtr packet)
{
	m_Packet = packet;
	return *this;
}

#ifdef _DEBUG
void TracePacket(const ProtoPacketPtr packet, const char* text)
{
	boost::mutex::scoped_lock lock(g_PacketMutex);
	packet->add_trace(text); 
}
#endif // _DEBUG
