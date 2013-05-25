/*
 * PluginLoader.h
 *
 *  Created on: Nov 15, 2011
 *      Author: root
 */

#ifndef PLUGINLOADER_H_
#define PLUGINLOADER_H_

#include "Types.h"
#include "Factory.h"
#include "ILog.h"
#include "ISettings.h"

#include <vector>

#ifdef WIN32
#include <windows.h>
#endif

namespace srv
{

class CPluginLoader
{

	//! Plugin init function
	typedef void (*TInitFunction)(cmn::TExecutorsFactory&, cmn::ILog&, cmn::ISettings&);

	//! Plugin uninit function
	typedef void (*TShutDownFunction)(void);

	//! Library handle
#ifdef WIN32
    typedef HINSTANCE 			THandle;
#else
    typedef void * 				THandle;
#endif

	//! Struct for pluggin handling
	struct CPlugin
	{
    	THandle					Handle;
		TInitFunction 			Init;
		TShutDownFunction 		Shutdown;
	};

	//! Type of the loaded plugins container
	typedef std::vector< CPlugin > 			TLoadedPlugins;

public:
	//! Plugins list type
	typedef std::vector< _tstring > 		TPlugins;

	CPluginLoader() = delete;
	CPluginLoader(cmn::TExecutorsFactory& Factory, const _tstring& sPath, const TPlugins& vecPlugins);
	virtual ~CPluginLoader();

	//! Load plugins
	void 									Load();

	//! Unload plugins
	void 									Free();

private:

	//! Refference to factory
	cmn::TExecutorsFactory&					m_Factory;

	//! Plugin path
	const _tstring&							m_sPath;

	//! Plugins list
	const TPlugins&							m_vecPluginNames;

	//! Loaded pluggins
	TLoadedPlugins							m_vecLoadedPlugins;
};

} /* namespace srv */
#endif /* PLUGINLOADER_H_ */
