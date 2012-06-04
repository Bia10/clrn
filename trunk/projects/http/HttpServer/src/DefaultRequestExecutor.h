/*
 * Request.h
 *
 *  Created on: Nov 7, 2011
 *      Author: root
 */

#ifndef DEFAULTREQUESTEXECUTOR_H_
#define DEFAULTREQUESTEXECUTOR_H_

#include "Types.h"
#include "IExecutor.h"

namespace srv
{

class CDefaultRequestExecutor : public IExecutor
{
public:
	CDefaultRequestExecutor();
	virtual ~CDefaultRequestExecutor();

	//! Executing
	virtual void 							Execute(const http::IHttpData& RequestInfo, http::IHttpData& Reply);

	//! Get info about interface
	virtual const TCHAR * 					Description();

	//! Checks posibility of execution request by this instance of IExecutor
	virtual const bool 						Match(const http::IHttpData& RequestInfo);

private:

	//! Check for cached reply
	virtual const bool 						GetCachedReply(http::IHttpData& Reply) const;

	//! Process request
	virtual void	 						ProcessRequest(const http::IHttpData& RequestInfo, http::IHttpData& Reply);

	//! Request
	const http::IHttpData * 				m_pRequest;

	//! Request path
	_tstring								m_sPath;

	//! Documents root directory
	_tstring								m_sDocumentsRoot;
};

} /* namespace srv */
#endif /* DEFAULTREQUESTEXECUTOR_H_ */
