#ifndef PluginLoader_h__
#define PluginLoader_h__

#include "JobFactory.h"
#include "ILog.h"
#include "IKernel.h"

#include <vector>

#ifdef WIN32
#include <windows.h>
#endif

class CPluginLoader
{	
	CPluginLoader();
	void operator = (CPluginLoader&);

	//! Plugin init function
	typedef void (*TInitFunction)(CJobFactory&, ILog&, IKernel&);

	//! Plugin uninit function
	typedef void (*TShutDownFunction)(CJobFactory&);

	//! Library handle
#ifdef WIN32
    typedef HINSTANCE 			THandle;
#else
    typedef void * 				THandle;
#endif

	//! Struct for plugin handling
	struct CPlugin
	{
    	THandle					Handle;
		TInitFunction 			Init;
		TShutDownFunction 		Shutdown;
	};

	//! Type of the loaded plugins container
	typedef std::vector<CPlugin> 			TLoadedPlugins;

public:
	//! Plugins list type
	typedef std::vector<std::string> 		TPlugins;


	CPluginLoader(ILog& logger, 
				IKernel& kernel,
				CJobFactory& Factory, 
				const std::string& sPath, 
				const TPlugins& vecPlugins);
	
	virtual ~CPluginLoader();

	//! Load plugins
	void 									Load();

	//! Shutdown plugins
	void 									Shutdown();

	//! Unload plugins
	void									Unload();

private:

	//! Logger reference
	ILog&									m_Log;

	//! Kernel reference
	IKernel&								m_Kernel;

	//! Refference to factory
	CJobFactory&							m_Factory;

	//! Plugin path
	const std::string&						m_sPath;

	//! Plugins list
	const TPlugins&							m_vecPluginNames;

	//! Loaded pluggins
	TLoadedPlugins							m_vecLoadedPlugins;
};

#endif // PluginLoader_h__
