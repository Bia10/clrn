/*
 * Status.h
 *
 *  Created on: Nov 7, 2011
 *      Author: root
 */

#ifndef STATUS_H_
#define STATUS_H_

#include "Types.h"

namespace cmn
{

//! Reply status
class CStatus
{
public:

	enum class StatusCode
	{
		eOk						= 200,
		eCreated 				= 201,
		eAccepted 				= 202,
		eNoContent 				= 204,
		eMultipleChoices 		= 300,
		eMovedPermanently 		= 301,
		eMovedTemporarily 		= 302,
		eNotModified 			= 304,
		eBadRequest 			= 400,
		eUnauthorized 			= 401,
		eForbidden 				= 403,
		eNotFound 				= 404,
		eInternalServerError 	= 500,
		eNotImplemented 		= 501,
		eBadGateway 			= 502,
		eServiceUnavailable 	= 503
	};

	CStatus(const StatusCode& eStatus);
	CStatus() = delete;

	static const std::string& 	ToString(const StatusCode& StatusCode);
	const _tstring& 			ToAnswer();
	const _tstring	 			ToHtml();
	void 						Set(const StatusCode& eStatus);
	const StatusCode& 			Get() const;

private:

	//! Status
	StatusCode					m_eStatus;

	//! Text of the answwer status
	_tstring	 				m_sAnswerStatus;

};

} /* namespace cmn */
#endif /* STATUS_H_ */
