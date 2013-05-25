/*
 * Registrator.h
 *
 *  Created on: Dec 17, 2011
 *      Author: root
 */

#ifndef REGISTRATOR_H_
#define REGISTRATOR_H_

#include "IExecutor.h"

//! Class for user registration routine
class CRegistrator : public srv::IExecutor
{
public:
	CRegistrator();

	//! Get info about interface
	virtual const TCHAR * 	Description();

	//! Checks posibility of execution request by this instance of IExecutor
	virtual const bool 		Match(const http::IHttpData& Request);

	//! Executing
	virtual void 			Execute(const http::IHttpData& Request, http::IHttpData& Reply);

	//! Virtual destructor
	virtual ~CRegistrator();
};

#endif /* REGISTRATOR_H_ */
