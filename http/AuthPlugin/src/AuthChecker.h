/*
 * AuthChecker.h
 *
 *  Created on: Nov 16, 2011
 *      Author: root
 */

#ifndef AUTHCHECKER_H_
#define AUTHCHECKER_H_

#include "IExecutor.h"

class CAuthChecker: public srv::IExecutor
{
public:
	CAuthChecker();
	virtual ~CAuthChecker();

	//! Get info about interface
	virtual const TCHAR * 	Description();

	//! Checks posibility of execution request by this instance of IExecutor
	virtual const bool 		Match(const http::IHttpData& Info);

	//! Executing
	virtual void 			Execute(const http::IHttpData& Info, http::IHttpData& Reply);

private:

	//! Validates session hash
	const bool 				IsHashValid(const _tstring& sHash);
};

#endif /* AUTHCHECKER_H_ */
