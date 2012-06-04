#ifndef Log_h__
#define Log_h__

#include "ILog.h"
#include "Config.h"

#include <ostream>

#include <boost/format.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/mutex.hpp>

//! Log interface
//!
//! \class CLog
//!
class CLog
	: public ILog
{
public:
	CLog(void);
	~CLog(void);

	//! Open log
	void		Open(const std::string& szSource);
	void		Open(const std::wstring& szSource);

	//! Close log
	void		Close();

	//! Set log module levels
	void		SetLogLevels(const std::vector<Level::Enum_t>& levels);
	
	//! Is logging enabled
	bool		IsEnabled(unsigned int module, const Level::Enum_t level) const;

	//! Write text
	ILog&		Error(const std::string& szText);
	ILog&		Error(const std::wstring& szText);
	ILog&		Warning(const std::string& szText);
	ILog&		Warning(const std::wstring& szText);
	ILog&		Trace(const std::string& szText);
	ILog&		Trace(const std::wstring& szText);
	ILog&		Debug(const std::string& szText);
	ILog&		Debug(const std::wstring& szText);

	ILog&		Error(const std::string& szFunction, const std::string& szText);
	ILog&		Error(const std::string& szFunction, const std::wstring& szText);
	ILog&		Warning(const std::string& szFunction, const std::string& szText);
	ILog&		Warning(const std::string& szFunction, const std::wstring& szText);
	ILog&		Trace(const std::string& szFunction, const std::string& szText);
	ILog&		Trace(const std::string& szFunction, const std::wstring& szText);
	ILog&		Debug(const std::string& szFunction, const std::string& szText);
	ILog&		Debug(const std::string& szFunction, const std::wstring& szText);

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

	//! Internal write
	void								Write(const Level::Enum_t level);

	//! Level to string
	const char*							Level2String(const Level::Enum_t level) const;

	//! Log output stream pointer
	std::ostream*						m_pStream;

	//! Log levels
	std::vector<Level::Enum_t>			m_Levels;

	//! Current message level
	Level::Enum_t						m_CurrentLevel;

	//! External stream
	bool								m_IsExternalStream;

	//! Format object
	boost::format						m_Format;

	//! Write mutex
	boost::mutex						m_Mutex;

	//! Open flag
	bool								m_IsOpened;

};

#endif // Log_h__
