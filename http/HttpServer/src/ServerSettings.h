/*
 * ServerSettings.h
 *
 *  Created on: Nov 15, 2011
 *      Author: root
 */

#ifndef SERVERSETTINGS_H_
#define SERVERSETTINGS_H_

#include "ISettings.h"

#include <boost/scoped_ptr.hpp>
#include <boost/property_tree/ptree_fwd.hpp>

namespace cmn
{

class CServerSettings : public ISettings
{
	//! Xml type
	typedef boost::property_tree::basic_ptree<
			std::basic_string< TCHAR >,
			std::basic_string< TCHAR >,
			boost::property_tree::detail::less_nocase< std::basic_string< TCHAR > >
	> TXmlNode;

	//! Xml pointer type
	typedef boost::scoped_ptr< TXmlNode > TXmlPtr;

public:
	CServerSettings();
	virtual ~CServerSettings();

	//! Get log source
	virtual const _tstring						GetLogSource() const;

	//! Get log level
	virtual const ILog::Level					GetLogLevel() const;

	//! Get server host
	virtual const _tstring						GetHost() const;

	//! Get server port
	virtual const std::size_t					GetPort() const;

	//! Get server number of threads
	virtual const std::size_t					GetNumberOfThreads() const;

	//! Get www root
	virtual const _tstring						GetDocRoot() const;

	//! Get plugins path
	virtual const _tstring						GetPluginsPath() const;

	//! Get plugins list
	virtual const std::vector< _tstring >		GetPluginsList() const;

	//! Load settings
	virtual void 								Load(const _tstring& sSettingsFileName);

	//! Get custom value
	virtual const _tstring						GetValue(const _tstring& sPath) const;

	//! Set custom value
	virtual void								SetValue(const _tstring& sPath, const _tstring& sValue);

	//! Save settings
	virtual void								Save();

private:

	//! Create default
	virtual void 								CreateDefault();

	//! Settings path
	_tstring									m_sPath;

	//! Settings xml node
	TXmlPtr										m_pxmlSettings;
};


//! Class singleton for settings instance
class Settings
{
public:
	static void Open(const _tstring& sSettingsFileName);
	static CServerSettings& Instance();
private:
	static CServerSettings ms_Settings;
};

} /* namespace cmn */
#endif /* SERVERSETTINGS_H_ */
