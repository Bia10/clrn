#ifndef Settings_h__
#define Settings_h__

#include "ILog.h"
#include "ProtoTablePtr.h"

#include <string>
#include <memory>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

//! Forward declarations
class DataBase;
class CDataTable;

//! Settings
//!
//! \class CSettings
//!
class CSettings
{
public: 
	CSettings(ILog& logger, DataBase& dataBase, data::Table& data);
	~CSettings(void);

	//! Save settings				
	void				Save();

	//! Load settings
	void				Load();

	//! Set value
	void				Set(const int module, const unsigned int value, const std::string& path);
	void				Set(const int module, const int value, const std::string& path);
	void				Set(const int module, const double value, const std::string& path);
	void				Set(const int module, const std::string& value, const std::string& path);
	void				Set(const int module, const std::wstring& value, const std::string& path);

	//! Get value
	void				Get(const int module, unsigned int& value, const std::string& path);
	void				Get(const int module, int& value, const std::string& path);
	void				Get(const int module, double& value, const std::string& path);
	void				Get(const int module, std::string& value, const std::string& path);
	void				Get(const int module, std::wstring& value, const std::string& path);
	
private:

	//! Implementation
	class Impl;
	boost::scoped_ptr<Impl>	m_pImpl;
};


#endif // Settings_h__