#ifndef ILog_h__
#define ILog_h__

#include "Config.h"

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
	virtual void		Open(const std::string& szSource, unsigned int module) = 0;
	virtual void		Open(const std::wstring& szSource, unsigned int module) = 0;

	//! Close log
	virtual void		Close() = 0;

	//! Set log module levels
	virtual void		SetLogLevels(const std::vector<Level::Enum_t>& levels) = 0;

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
	virtual ILog&		operator % (const unsigned int value)	= 0;
	virtual ILog&		operator % (const int value)			= 0;
	virtual ILog&		operator % (const double value)			= 0;
	virtual ILog&		operator % (const unsigned long value)	= 0;
	virtual ILog&		operator % (const long value)			= 0;

	virtual ILog&		operator % (const std::string& value)	= 0;
	virtual ILog&		operator % (const std::wstring& value)	= 0;

	//! Make scoped log
	virtual ScopedLogPtr MakeScopedLog(unsigned int module, const std::string& func)	= 0;

	//! Destructor
	virtual ~ILog() {}
};

#endif // ILog_h__