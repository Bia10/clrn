/*
 * PhpExecutor.h
 *
 *  Created on: Nov 15, 2011
 *      Author: root
 */

#ifndef PHPEXECUTOR_H_
#define PHPEXECUTOR_H_

#include "IExecutor.h"
#include "ILog.h"

#include <boost/thread/mutex.hpp>


class CPhpExecutor: public srv::IExecutor
{
public:
	//! Scoped php initialization
	class CScopedInit
	{
	public:
		CScopedInit(boost::mutex& mxLib, CPhpExecutor& Instance);
		~CScopedInit();
	private:
		boost::mutex&		m_mxPhpLib;
		CPhpExecutor&		m_Instance;
	};

	CPhpExecutor();
	virtual ~CPhpExecutor();

	//! Get info about interface
	virtual const TCHAR * 						Description();

	//! Checks posibility of execution request by this instance of IExecutor
	virtual const bool 							Match(const http::IHttpData& Request);

	//! Executing
	virtual void 								Execute(const http::IHttpData& Request, http::IHttpData& Reply);

private:

	//! Set up global variable
	void 										SetVariable(const _tstring& sName, const _tstring& sValue);

	//! Read variable
	void 										GetVariable(const _tstring& sName, _tstring& sValue);

	//! Initialization
	void 										Init();

	//! Uninitialization
	void 										UnInit();

	//! Pointer to the input request
	const http::IHttpData *						m_pRequest;

	//! Pointer to the output reply
	http::IHttpData *							m_pReply;

	//! Mutex for library execution
	static boost::mutex							ms_mxPhpLib;
};

#endif /* PHPEXECUTOR_H_ */
