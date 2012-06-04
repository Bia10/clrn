/*
 * Request.h
 *
 *  Created on: Nov 7, 2011
 *      Author: root
 */

#ifndef IEXECUTOR_H_
#define IEXECUTOR_H_

#include "IHttpData.h"

namespace srv
{

//! Interface of the request object
class IExecutor
{
public:

	//! Get info about interface
	virtual const TCHAR * 	Description() = 0;

	//! Checks posibility of execution request by this instance of IExecutor
	virtual const bool 		Match(const http::IHttpData&) = 0;

	//! Executing
	virtual void 			Execute(const http::IHttpData&, http::IHttpData&) = 0;

	//! Virtual destructor
	virtual				 	~IExecutor() {}

};

} // namespace srv

#endif /* IEXECUTOR_H_ */
