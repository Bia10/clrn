#ifndef Log_h__
#define Log_h__

#include "ILog.h"
#include "Config.h"

#include <ostream>
#include <vector>

#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/mutex.hpp>

namespace log_details
{

//! Log stream implementation
class LogStream
{
	//! Stream information
	struct StreamInfo
	{
		std::ostream*		stream;
		bool				created;
		ILog::Level::Enum_t	lvl;
	};

	//! Stream vector
	typedef std::vector<StreamInfo>			Streams;

public:

	//! Pointer type
	typedef boost::shared_ptr<LogStream> Ptr;

	LogStream()
	{

	}

	~LogStream()
	{
		Streams::const_iterator it = m_Streams.begin();
		const Streams::const_iterator itEnd = m_Streams.end();
		for (; it != itEnd; ++it)
		{
			if (it->created)
				delete it->stream;
		}
	}

	bool IsEnabled(const ILog::Level::Enum_t level) const
	{
		Streams::const_iterator it = m_Streams.begin();
		const Streams::const_iterator itEnd = m_Streams.end();
		for (; it != itEnd; ++it)
		{
			if (it->lvl >= level)
				return true;
		}

		return false;
	}

	void AddStream(std::ostream* stream, const ILog::Level::Enum_t level, const bool created)
	{
		if (!stream)
			return;

		StreamInfo info;
		info.created = created;
		info.lvl = level;
		info.stream = stream;

		m_Streams.push_back(info);
	}

	void Write (const std::string& text, const ILog::Level::Enum_t level)
	{
		Streams::const_iterator it = m_Streams.begin();
		const Streams::const_iterator itEnd = m_Streams.end();
		for (; it != itEnd; ++it)
		{
			if (it->lvl >= level)
			{
				*it->stream << text;
				it->stream->flush();
			}
		}
	}

private:

	//! Streams bound to this log stream
	Streams			m_Streams;
};

} // namespace log_details

//! Logger interface
//!
//! \class CLog
//!
class CLog
	: public ILog
{
	//! Stream vector
	typedef std::vector<log_details::LogStream::Ptr>	Streams;

	//! Stream path to pointer container
	typedef std::map<std::wstring, std::ostream*>		StreamIndexes;

public:
	CLog(void);
	~CLog(void);

	//! Open log
	void		Open(const std::string& szSource, unsigned int module, const Level::Enum_t level);
	void		Open(const std::wstring& szSource, unsigned int module, const Level::Enum_t level);

	//! Close log
	void		Close();

	//! Is logging enabled
	bool		IsEnabled(unsigned int module, const Level::Enum_t level) const;

	//! Write text
	ILog&		Error(unsigned int module, const std::string& szText);
	ILog&		Error(unsigned int module, const std::wstring& szText);
	ILog&		Warning(unsigned int module, const std::string& szText);
	ILog&		Warning(unsigned int module, const std::wstring& szText);
	ILog&		Trace(unsigned int module, const std::string& szText);
	ILog&		Trace(unsigned int module, const std::wstring& szText);
	ILog&		Debug(unsigned int module, const std::string& szText);
	ILog&		Debug(unsigned int module, const std::wstring& szText);

	ILog&		Error(unsigned int module, const std::string& szFunction, const std::string& szText);
	ILog&		Error(unsigned int module, const std::string& szFunction, const std::wstring& szText);
	ILog&		Warning(unsigned int module, const std::string& szFunction, const std::string& szText);
	ILog&		Warning(unsigned int module, const std::string& szFunction, const std::wstring& szText);
	ILog&		Trace(unsigned int module, const std::string& szFunction, const std::string& szText);
	ILog&		Trace(unsigned int module, const std::string& szFunction, const std::wstring& szText);
	ILog&		Debug(unsigned int module, const std::string& szFunction, const std::string& szText);
	ILog&		Debug(unsigned int module, const std::string& szFunction, const std::wstring& szText);

	//! Write formatted argument
	ILog&		operator % (const unsigned int value);
	ILog&		operator % (const int value);
	ILog&		operator % (const double value);
	ILog&		operator % (const unsigned long value);
	ILog&		operator % (const long value);

	ILog&		operator % (const std::string& value);
	ILog&		operator % (const std::wstring& value);


	//! Make scoped log
	ScopedLogPtr MakeScopedLog(unsigned int module, const std::string& szFunction);

private:

	//!  Internal write
	void		Write(const Level::Enum_t level);

	//! Level to string
	const char*	Level2String(const Level::Enum_t level) const;

	//! Log output stream pointers
	Streams								m_Streams;

	//! Current message level
	Level::Enum_t						m_CurrentLevel;

	//! Format object
	boost::format						m_Format;

	//! Write mutex
	boost::mutex						m_Mutex;

	//! Open flag
	bool								m_IsOpened;

	//! Current module id
	unsigned int						m_CurrentModule;

	//! Stream indexes
	StreamIndexes						m_StreamIndexes;

	//! Temp stream
	std::ostringstream					m_TempStream;
};

#endif // Log_h__
