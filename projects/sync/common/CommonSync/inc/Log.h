#ifndef Log_h__
#define Log_h__

#include "ILog.h"
#include "Config.h"

#include <ostream>
#include <vector>

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
	//! Stream vector
	typedef std::vector<std::ostream*>	Streams;

	//! Stream open flags(created or opened)
	typedef std::vector<bool>			StreamFlags;


public:
	CLog(void);
	~CLog(void);

	//! Open log
	void		Open(const std::string& szSource, unsigned int module);
	void		Open(const std::wstring& szSource, unsigned int module);

	//! Close log
	void		Close();

	//! Set log module levels
	void		SetLogLevels(const std::vector<Level::Enum_t>& levels);
	
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
	void								Write(const Level::Enum_t level);

	//! Level to string
	const char*							Level2String(const Level::Enum_t level) const;

	//! Log output stream pointers
	Streams								m_Streams;

	//! Stream flags
	StreamFlags							m_CreatedStreams;

	//! Log levels
	std::vector<Level::Enum_t>			m_Levels;

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

};

#endif // Log_h__
