/*
 * PluginLoader.cpp
 *
 *  Created on: Nov 15, 2011
 *      Author: root
 */

#include "PluginLoader.h"
#include "ServerLogger.h"
#include "Exception.h"
#include "ServerSettings.h"


#ifdef WIN32
#include <direct.h>
#else
#include <sys/types.h>
#include <dlfcn.h>
#endif

#include <boost/filesystem/operations.hpp>


namespace srv
{

CPluginLoader::CPluginLoader(cmn::TExecutorsFactory& Factory, const _tstring& sPath, const TPlugins& vecPlugins) :
		m_Factory(Factory),
		m_sPath(sPath),
		m_vecPluginNames(vecPlugins)
{
	SCOPED_LOG_FUNCTION
}

CPluginLoader::~CPluginLoader()
{
	SCOPED_LOG_FUNCTION
}

void CPluginLoader::Load()
{
	SCOPED_LOG_FUNCTION

    for (const _tstring& sPlugin : m_vecPluginNames)
    {
    	TRY_EXCEPTIONS
    	{
    		THandle pPluginHandle = NULL;

    		// Detecting correct library name
			_tstring sLibraryName;

#ifndef WIN32
			if (sPlugin.size() < 3 || sPlugin.substr(0, 3) != _T("lib"))
				sLibraryName.append(_T("lib"));
#endif

			sLibraryName.append(sPlugin);

			const _tstring::size_type nDot = sLibraryName.find_last_of('.');
			if (_tstring::npos != nDot)
			{
				sLibraryName.resize(nDot);
			}

#ifdef WIN32
			sLibraryName.append(_T(".dll"));
#else
    		sLibraryName.append(_T(".so"));
#endif

			// Detecting full path
			_tstring sFullPath = boost::filesystem::system_complete(m_sPath).string();
			sFullPath.append(sLibraryName);

			cmn::Logger::Instance().Write(cmn::CFmtWrap(_T("Loading plugin... Complete path [%s].")) % sFullPath);

			if (!boost::filesystem::exists(sFullPath))
			{
				cmn::Logger::Instance().Write(cmn::CFmtWrap(_T("Path [%s] does not exists."), cmn::ILog::Level::eError) % sFullPath);
				continue;
			}

			// Loading library
#ifdef WIN32
			pPluginHandle = LoadLibrary(sFullPath.c_str());
			if (!pPluginHandle)
			{
				cmn::Logger::Instance().Write(cmn::CFmtWrap(_T("Failed to load library [%s]."), cmn::ILog::Level::eError) % sFullPath);
				continue;
			}
#else
			pPluginHandle = dlopen(sFullPath.c_str(), RTLD_LAZY);
			if (!pPluginHandle)
			{
				cmn::Logger::Instance().Write(cmn::CFmtWrap(_T("Failed to load library [%s] Reason [%s]."), cmn::ILog::Level::eError) % sFullPath % dlerror());
				continue;
			}
#endif

			cmn::Logger::Instance().Write(cmn::CFmtWrap(_T("Loading symbols for [%s].")) % sPlugin);

			// Loading symbols
#ifdef WIN32
			TInitFunction pfnInit = reinterpret_cast< TInitFunction > (GetProcAddress(pPluginHandle, "Init"));
			if (!pfnInit)
			{
				cmn::Logger::Instance().Write(cmn::CFmtWrap(_T("Failed to load library [%s] function [%s]."), cmn::ILog::Level::eError) % sFullPath % _T("Init"));
				continue;
			}

			TShutDownFunction pfnShutdown = reinterpret_cast< TShutDownFunction > (GetProcAddress(pPluginHandle, "Shutdown"));
			if (!pfnShutdown)
			{
				cmn::Logger::Instance().Write(cmn::CFmtWrap(_T("Failed to load library [%s] function [%s]."), cmn::ILog::Level::eError) % sFullPath % _T("Shutdown"));
				continue;
			}
#else
			// Reset errors
			dlerror();

			TInitFunction pfnInit = reinterpret_cast< TInitFunction > (dlsym(pPluginHandle, "Init"));
			if (!pfnInit)
			{
				cmn::Logger::Instance().Write(cmn::CFmtWrap(_T("Failed to load library [%s] function [%s] Reason [%s]."), cmn::ILog::Level::eError) % sFullPath % _T("Init") % dlerror());
				continue;
			}

			TShutDownFunction pfnShutdown = reinterpret_cast< TShutDownFunction > (dlsym(pPluginHandle, "Shutdown"));
			if (!pfnShutdown)
			{
				cmn::Logger::Instance().Write(cmn::CFmtWrap(_T("Failed to load library [%s] function [%s]."), cmn::ILog::Level::eError) % sFullPath % _T("Shutdown"));
				continue;
			}
#endif
			cmn::Logger::Instance().Write(cmn::CFmtWrap(_T("Initializing [%s].")) % sPlugin);

			(*pfnInit)(m_Factory, cmn::Logger::Instance(), cmn::Settings::Instance());

			CPlugin Plugin;
			Plugin.Handle		= pPluginHandle;
			Plugin.Init 		= pfnInit;
			Plugin.Shutdown 	= pfnShutdown;

			m_vecLoadedPlugins.push_back(Plugin);

			cmn::Logger::Instance().Write(cmn::CFmtWrap(_T("Plugin [%s] successfully loaded.")) % sPlugin);
    	}
		CATCH_IGNORE_EXCEPTIONS_LOG(cmn::Logger::Instance());
    }

}

void CPluginLoader::Free()
{
	SCOPED_LOG_FUNCTION

	for (CPlugin& Plugin : m_vecLoadedPlugins)
	{
		TRY_EXCEPTIONS
		{
			(*Plugin.Shutdown)();

#ifdef WIN32
			FreeLibrary(Plugin.Handle);
#else
			dlclose(Plugin.Handle);
#endif
		}
		CATCH_IGNORE_EXCEPTIONS_LOG(cmn::Logger::Instance());
	}
}

} /* namespace srv */
