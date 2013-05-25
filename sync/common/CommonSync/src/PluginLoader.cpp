#include "stdafx.h"
#include "PluginLoader.h"
#include "FileSystem.h"

#ifdef WIN32
#include <direct.h>
#else
#include <sys/types.h>
#include <dlfcn.h>
#endif


CPluginLoader::CPluginLoader(ILog& logger, IKernel& kernel, CJobFactory& Factory, const std::string& sPath, const TPlugins& vecPlugins)
	: m_Log(logger)
	, m_Kernel(kernel)
	, m_Factory(Factory)
	, m_sPath(sPath)
	, m_vecPluginNames(vecPlugins)
{
	SCOPED_LOG(m_Log);
}

CPluginLoader::~CPluginLoader()
{
	SCOPED_LOG(m_Log);
	
	Shutdown();
	Unload();
}

void CPluginLoader::Load()
{
	SCOPED_LOG(m_Log);

    BOOST_FOREACH(const std::string& sPlugin, m_vecPluginNames)
    {
    	TRY
    	{
    		THandle pPluginHandle = NULL;

    		// Detecting correct library name
			std::string sLibraryName;

#ifndef WIN32
			if (sPlugin.size() < 3 || sPlugin.substr(0, 3) != "lib")
				sLibraryName.append("lib");
#endif

			sLibraryName.append(sPlugin);

			const std::string::size_type nDot = sLibraryName.find_last_of('.');
			if (std::string::npos != nDot)
			{
				sLibraryName.resize(nDot);
			}

#ifdef WIN32
			sLibraryName.append(".dll");
#else
    		sLibraryName.append(".so");
#endif

			// Detecting full path
			std::string sFullPath = fs::FullPath(m_sPath);
			sFullPath.append(sLibraryName);

			LOG_TRACE("Loading plugin... Complete path [%s].") % sFullPath;

			if (!fs::Exists(sFullPath))
			{
				LOG_ERROR("Path [%s] does not exists.") % sFullPath;
				continue;
			}

			// Loading library
#ifdef WIN32
			pPluginHandle = LoadLibraryA(sFullPath.c_str());
			if (!pPluginHandle)
			{
				LOG_ERROR("Failed to load library [%s].") % sFullPath;
				continue;
			}
#else
			pPluginHandle = dlopen(sFullPath.c_str(), RTLD_LAZY);
			if (!pPluginHandle)
			{
				LOG_ERROR("Failed to load library [%s] Reason [%s].") % sFullPath % dlerror();
				continue;
			}
#endif

			LOG_TRACE("Loading symbols for [%s].") % sPlugin;

			// Loading symbols
#ifdef WIN32
			TInitFunction pfnInit = reinterpret_cast< TInitFunction > (GetProcAddress(pPluginHandle, "Init"));
			if (!pfnInit)
			{
				LOG_ERROR("Failed to load library [%s] function [%s].") % sLibraryName % "Init";
				continue;
			}

			TShutDownFunction pfnShutdown = reinterpret_cast< TShutDownFunction > (GetProcAddress(pPluginHandle, "Shutdown"));
			if (!pfnShutdown)
			{
				LOG_ERROR("Failed to load library [%s] function [%s].") % sLibraryName % "Shutdown";
				continue;
			}
#else
			// Reset errors
			dlerror();

			TInitFunction pfnInit = reinterpret_cast< TInitFunction > (dlsym(pPluginHandle, "Init"));
			if (!pfnInit)
			{
				LOG_ERROR("Failed to load library [%s] function [%s], reason [%s]") % sLibraryName % "Init" % dlerror();
				continue;
			}

			TShutDownFunction pfnShutdown = reinterpret_cast< TShutDownFunction > (dlsym(pPluginHandle, "Shutdown"));
			if (!pfnShutdown)
			{
				LOG_ERROR("Failed to load library [%s] function [%s], reason [%s]") % sLibraryName % "Shutdown" % dlerror();
				continue;
			}
#endif
			LOG_TRACE("Initializing [%s].") % sPlugin;

			(*pfnInit)(m_Factory, m_Log, m_Kernel);

			CPlugin Plugin;
			Plugin.Handle		= pPluginHandle;
			Plugin.Init 		= pfnInit;
			Plugin.Shutdown 	= pfnShutdown;

			m_vecLoadedPlugins.push_back(Plugin);

			LOG_TRACE("Plugin [%s] successfully loaded.") % sPlugin;
    	}
		CATCH_IGNORE_EXCEPTIONS(m_Log, "Load plugin failed")
    }
}

void CPluginLoader::Shutdown()
{
	SCOPED_LOG(m_Log);

	BOOST_FOREACH(CPlugin& Plugin, m_vecLoadedPlugins)
	{
		TRY
		{
			(*Plugin.Shutdown)(m_Factory);
		}
		CATCH_IGNORE_EXCEPTIONS(m_Log, "Free plugin failed")
	}
}

void CPluginLoader::Unload()
{
	SCOPED_LOG(m_Log);

	BOOST_FOREACH(CPlugin& Plugin, m_vecLoadedPlugins)
	{

#ifdef WIN32
		FreeLibrary(Plugin.Handle);
#else
		dlclose(Plugin.Handle);
#endif
	}

	m_vecLoadedPlugins.clear();
}

