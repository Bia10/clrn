/*
 * Authorizer.h
 *
 *  Created on: Nov 16, 2011
 *      Author: root
 */

#ifndef AUTHORIZER_H_
#define AUTHORIZER_H_

#include <IExecutor.h>

class CAuthorizer: public srv::IExecutor
{
public:
	CAuthorizer();
	virtual ~CAuthorizer();

	//! Get info about interface
	virtual const TCHAR * 	Description();

	//! Checks posibility of execution request by this instance of IExecutor
	virtual const bool 		Match(const http::IHttpData& Info);

	//! Executing
	virtual void 			Execute(const http::IHttpData& Info, http::IHttpData& Reply);

};

#endif /* AUTHORIZER_H_ */
