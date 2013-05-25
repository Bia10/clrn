/*
 * ISettings.h
 *
 *  Created on: Nov 15, 2011
 *      Author: root
 */

#ifndef ISETTINGS_H_
#define ISETTINGS_H_

#include "Types.h"
#include "ILog.h"

#include <vector>

namespace cmn
{

//! Interface of the settings
class ISettings
{
public:

	ISettings() {}
	virtual ~ISettings() {}

	//! Get log source
	virtual const _tstring						GetLogSource() const = 0;

	//! Get log level
	virtual const ILog::Level					GetLogLevel() const = 0;

	//! Get server host
	virtual const _tstring						GetHost() const = 0;

	//! Get server port
	virtual const std::size_t					GetPort() const = 0;

	//! Get server number of threads
	virtual const std::size_t					GetNumberOfThreads() const = 0;

	//! Get www root
	virtual const _tstring						GetDocRoot() const = 0;

	//! Get plugins path
	virtual const _tstring						GetPluginsPath() const = 0;

	//! Get plugins list
	virtual const std::vector< _tstring >		GetPluginsList() const = 0;

	//! Get custom value
	virtual const _tstring						GetValue(const _tstring& sPath) const = 0;

	//! Set custom value
	virtual void								SetValue(const _tstring& sPath, const _tstring& sValue) = 0;

	//! Save settings
	virtual void								Save() = 0;
};

} /* namespace cmn */
#endif /* ISETTINGS_H_ */
