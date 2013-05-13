#ifndef ILog_h__
#define ILog_h__

#include "Conversion.h"

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>

//! Scoped log macro
#define SCOPED_LOG(log) const ILog::ScopedLogPtr __scopedLogPtr = log.MakeScopedLog(CURRENT_MODULE_ID, __FUNCTION__);

//! Logging macros
#define LOG_ERROR(message)		if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Error))		\
	m_Log.Error(CURRENT_MODULE_ID, __FUNCTION__, message)
#define LOG_WARNING(message)	if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Warning))	\
	m_Log.Warning(CURRENT_MODULE_ID, __FUNCTION__, message)
#define LOG_TRACE(message)		if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Trace))		\
	m_Log.Trace(CURRENT_MODULE_ID, __FUNCTION__, message)
#define LOG_DEBUG(message)		if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Debug))		\
	m_Log.Debug(CURRENT_MODULE_ID, __FUNCTION__, message)

//! Forward declarations
class CScopedLog;
class LogAnyHolder;

//! Logger interface
//!
//! \class ILog
//!
class ILog
{
public:

	//! Scoped log ptr type
	typedef boost::shared_ptr<CScopedLog> ScopedLogPtr;

	//! Log levels
	struct Level
	{
		enum Enum_t
		{
			None	= 0,
			Error	= 1,
			Warning	= 2,
			Trace	= 3,
			Debug	= 4
		};
	};

	//! Open log
	virtual void		Open(const std::string& szSource, unsigned int module, const Level::Enum_t level) = 0;
	virtual void		Open(const std::wstring& szSource, unsigned int module, const Level::Enum_t level) = 0;

	//! Close log
	virtual void		Close() = 0;

	//! Is logging enabled
	virtual bool		IsEnabled(unsigned int module, const Level::Enum_t level) const = 0;

	//! Write text
	virtual ILog&		Error(unsigned int module, const std::string& text)			= 0;
	virtual ILog&		Error(unsigned int module, const std::wstring& text)		= 0;
	virtual ILog&		Warning(unsigned int module, const std::string& text)		= 0;
	virtual ILog&		Warning(unsigned int module, const std::wstring& text)		= 0;
	virtual ILog&		Trace(unsigned int module, const std::string& text)			= 0;
	virtual ILog&		Trace(unsigned int module, const std::wstring& text)		= 0;
	virtual ILog&		Debug(unsigned int module, const std::string& text)			= 0;
	virtual ILog&		Debug(unsigned int module, const std::wstring& text)		= 0;

	virtual ILog&		Error(unsigned int module, const std::string& func,	const std::string& text)		= 0;
	virtual ILog&		Error(unsigned int module, const std::string& func,	const std::wstring& text)		= 0;
	virtual ILog&		Warning(unsigned int module, const std::string& func, const std::string& text)		= 0;
	virtual ILog&		Warning(unsigned int module, const std::string& func, const std::wstring& text)		= 0;
	virtual ILog&		Trace(unsigned int module, const std::string& func,	const std::string& text)		= 0;
	virtual ILog&		Trace(unsigned int module, const std::string& func,	const std::wstring& text)		= 0;
	virtual ILog&		Debug(unsigned int module, const std::string& func,	const std::string& text)		= 0;
	virtual ILog&		Debug(unsigned int module, const std::string& func,	const std::wstring& text)		= 0;

	//! Write formatted argument
	virtual ILog&		operator % (const LogAnyHolder& value)	= 0;

	//! Make scoped log
	virtual ScopedLogPtr MakeScopedLog(unsigned int module, const std::string& func)	= 0;

	//! Destructor
	virtual ~ILog() {}
};

//! Any value holder
class LogAnyHolder
{
public:

    template<typename T>
    LogAnyHolder(const T& arg)
    {
        m_Buffer = conv::cast<std::string>(arg);
    }

    template<typename T>
    LogAnyHolder(const std::vector<T>& arg)
    {
        if (arg.empty())
            return;

        std::ostringstream oss;
        for (const T& val : arg)
            oss << val << ",";

        m_Buffer = oss.str().substr(0, std::size_t(oss.tellp()) - 1);
    }

    operator std::string() const
    {
        return m_Buffer;
    }
private:
    std::string m_Buffer;
};

#endif // ILog_h__