/*
 * PhpExecutor.cpp
 *
 *  Created on: Nov 15, 2011
 *      Author: root
 */


#include "Factory.h"
#include "ILog.h"
#include "ISettings.h"
#include "PhpExecutor.h"

#include <stdexcept>
#include <fstream>

#include <php/sapi/embed/php_embed.h>
#include <zend_API.h>

#include <boost/filesystem.hpp>

#ifdef __cplusplus
extern "C" {
#endif

void Init(cmn::TExecutorsFactory&, cmn::ILog&, cmn::ISettings&);
void Shutdown(void);

#ifdef __cplusplus
}
#endif

//! Global pointers
cmn::ILog * g_pLog = NULL;
cmn::ISettings * g_pSettings = NULL;
http::IHttpData * g_pReply = NULL;

int OutputHandle(const char *szMessage, std::size_t nLength)
{
	g_pReply->AppendContent(szMessage, nLength);

	return 0;
}

void LogHandle(const char *szMessage)
{
	g_pLog->Write(szMessage, cmn::ILog::Level::eMessage);
}

void ErrorHandle(int nType, const char * szFormat, ...)
{
	g_pLog->Write(szFormat, cmn::ILog::Level::eMessage);
}

void LogMessageHandle(char * szMessage)
{
	g_pLog->Write(szMessage, cmn::ILog::Level::eMessage);
}

int PhpStartupHandler(sapi_module_struct *mod)
{
    return php_module_startup(mod, NULL, 0);
}

sapi_module_struct g_foobar_php_module = {
	"foobar",                       /* name */
	"FOOBAR-PHP Interface",         /* pretty name */
	PhpStartupHandler,            	/* startup */
	php_module_shutdown_wrapper,    /* shutdown */
	NULL,                           /* activate */
	NULL,                           /* deactivate */
	OutputHandle,           		/* unbuffered write */
	NULL,                           /* flush */
	NULL,                           /* get uid */
	NULL,                           /* getenv */
	ErrorHandle,                    /* error handler */
	NULL,                           /* header handler */
	NULL,                           /* send headers handler */
	NULL,                           /* send header handler */
	NULL,                           /* read POST data */
	NULL,                           /* read Cookies */
	NULL,                           /* register server variables */
	LogMessageHandle,         		/* Log message */

	STANDARD_SAPI_MODULE_PROPERTIES
};

void Init(cmn::TExecutorsFactory& Factory, cmn::ILog& Logger, cmn::ISettings& Settings)
{
	Factory.Add< CPhpExecutor >(50);

	g_pLog = &Logger;
	g_pSettings = &Settings;
}

void Shutdown()
{

}


void CPhpExecutor::Init()
{
	zend_llist global_vars;

#ifdef ZTS /* zend threat safety */
	zend_compiler_globals *compiler_globals;
	zend_executor_globals *executor_globals;
	php_core_globals *core_globals;
	sapi_globals_struct *sapi_globals;
	void ***tsrm_ls;

    tsrm_startup(1, 1, 0, NULL);

    compiler_globals = ts_resource(compiler_globals_id);
    executor_globals = ts_resource(executor_globals_id);
    core_globals = ts_resource(core_globals_id);
    sapi_globals = ts_resource(sapi_globals_id);
    tsrm_ls = ts_resource(0);
    *ptsrm_ls = tsrm_ls;
#endif

    sapi_startup(&g_foobar_php_module);

    if (g_foobar_php_module.startup(&g_foobar_php_module) == FAILURE)
    {
    	throw std::runtime_error("Failed to initialize PHP library.");
    }

    g_foobar_php_module.executable_location = (char *)__func__;

    zend_llist_init(&global_vars, sizeof(char *), NULL, 0);

    if (php_request_startup(TSRMLS_C) == FAILURE)
    {
    	php_module_shutdown(TSRMLS_C);
    	throw std::runtime_error("Failed to initialize PHP library.");
    }

    SG(headers_sent) = 1;
    //SG(headers_sent)
    //SG(request_info)post_data.no_headers = 1;
    php_register_variable("PHP_SELF", "-", NULL TSRMLS_CC);
}

boost::mutex CPhpExecutor::ms_mxPhpLib;

CPhpExecutor::CPhpExecutor()
{

}

CPhpExecutor::~CPhpExecutor()
{

}


CPhpExecutor::CScopedInit::CScopedInit(boost::mutex & mxLib, CPhpExecutor & Instance) :
		m_mxPhpLib(mxLib),
		m_Instance(Instance)
{
	m_mxPhpLib.lock();
	m_Instance.Init();
}


CPhpExecutor::CScopedInit::~CScopedInit()
{
	m_Instance.UnInit();
	m_mxPhpLib.unlock();
}


void CPhpExecutor::UnInit()
{
  	php_request_shutdown((void *) 0);
	php_module_shutdown(TSRMLS_C);
	sapi_shutdown();
#ifdef ZTS
    tsrm_shutdown();
#endif
}

const TCHAR *CPhpExecutor::Description()
{
	static const _tstring s_sDesription = _T("Php executor.");
	return s_sDesription.c_str();
}



const bool CPhpExecutor::Match(const http::IHttpData & Request)
{
	const _tstring& sExtension = Request.GetRequestExtension();

	if (sExtension != _T("php"))
		return false;

	_tstring sPath = Request.GetRequestPath();

	// Full path
	_tstring sFullPath = g_pSettings->GetDocRoot();
	sFullPath.append(sPath);

	// Open script for reading
	_tifstream ifs(sFullPath.c_str(), std::ios::in);

	if (!ifs)
	{
		throw std::runtime_error("Failed to open php file script.");
	}

	// Reading header
	_tstring sPHPHeader(5, '\0');
	ifs.read(const_cast< char * > (sPHPHeader.c_str()), 5);
	ifs.seekg (0, std::ios::beg);

	if (sPHPHeader != _T("<?php"))
	{
		return false;
	}
	return true;
}



void CPhpExecutor::Execute(const http::IHttpData & Request, http::IHttpData & Reply)
{
	CScopedInit PhpInit(ms_mxPhpLib, *this);

	_tstring sPath = Request.GetRequestPath();

	// Full path
	_tstring sFullPath = g_pSettings->GetDocRoot();
	sFullPath.append(sPath);

	// Open script for reading
	_tifstream ifs(sFullPath.c_str(), std::ios::in);

	if (!ifs)
	{
		throw std::runtime_error("Failed to open php file script.");
	}

	const boost::filesystem::path pthFile(sFullPath);

	ifs.seekg (0, std::ios::end);
	const std::size_t nFileSize = ifs.tellg();
	ifs.seekg (0, std::ios::beg);

	std::vector< TCHAR > vecScript(nFileSize, '\0');

	ifs.read(&vecScript.front(), nFileSize);
	vecScript.push_back('\0');

	// Getting current directory from script file name and generating function text to set it up
	_tostringstream ossPhpSetPathText;
	ossPhpSetPathText << _T("chdir('") << pthFile.branch_path().c_str() << _T("');");

	// Executing script
	zend_first_try
	{
		// Setting up current directory
	    if (zend_eval_string(const_cast< char * > (ossPhpSetPathText.str().c_str()), NULL, const_cast< char * > (__func__)) == FAILURE)
	    {
	    	throw std::runtime_error("Syntax error in set path PHP script.");
	    }

	    // Seting up global variable $HTTP_RAW_POST_DATA
	    const _tstring sPostData = Request.GetPostData();
	   /* if (!sPostData.empty())
	    {
	    	SetVariable(_T("HTTP_RAW_POST_DATA"), sPostData);
	    }*/

	    SG(request_info).raw_post_data = const_cast< char * > (sPostData.c_str());
	    SG(request_info).raw_post_data_length = sPostData.size();

	    // Setting up php output
	    g_pReply = &Reply;


	    // Executing script
	    if (zend_eval_string(&vecScript[5], NULL, const_cast< char * > (__func__)) == FAILURE)
	    {
	    	throw std::runtime_error("Syntax error in PHP script.");
	    }
	}
	zend_catch
	{
		if (!g_pLog->IsEnabled())
			return;

		_tostringstream ossMessage;
		const _tstring sPostData = Request.GetPostData();
		ossMessage << _T("PHP execute error. Script: ") << &vecScript[5] << _T(" Data: ") << sPostData;
		g_pLog->Write(ossMessage.str(), cmn::ILog::Level::eError);
	}
	zend_end_try();
}


void CPhpExecutor::SetVariable(const _tstring & sName, const _tstring & sValue)
{
	zval *var;
	MAKE_STD_ZVAL(var);
	ZVAL_STRINGL(var, const_cast< char * > (sValue.c_str()), sValue.size(), 1);
	ZEND_SET_GLOBAL_VAR(const_cast< char * > (sName.c_str()), var);
}



void CPhpExecutor::GetVariable(const _tstring & sName, _tstring & sValue)
{
    zval **data = NULL;
    if (zend_hash_find(&EG(symbol_table), const_cast< char * > (sName.c_str()), sName.size() + 1, (void **)&data) == FAILURE)
    {
    	_tostringstream ossError;
    	ossError << sName << "  not found in $GLOBALS";
    	throw std::runtime_error(ossError.str());
    }

    if (data == NULL)
    {
    	sValue = _T("NULL");
        return;
    }

    convert_to_string(*data);

    sValue.assign(Z_STRVAL(**data), Z_STRLEN(**data));
}






