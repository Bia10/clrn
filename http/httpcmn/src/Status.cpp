/*
 * Status.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: root
 */

#include "Status.h"

#include <sstream>

namespace cmn
{

namespace StatusStrings{
	static const _tstring ok =
	  "OK";
	static const _tstring created =
	  "Created";
	static const _tstring accepted =
	  "Accepted";
	static const _tstring no_content =
	  "No Content";
	static const _tstring multiple_choices =
	  "Multiple Choices";
	static const _tstring moved_permanently =
	  "Moved Permanently";
	static const _tstring moved_temporarily =
	  "Moved Temporarily";
	static const _tstring not_modified =
	  "Not Modified";
	static const _tstring bad_request =
	  "Bad Request";
	static const _tstring unauthorized =
	  "Unauthorized";
	static const _tstring forbidden =
	  "Forbidden";
	static const _tstring not_found =
	  "Not Found";
	static const _tstring internal_server_error =
	  "Internal Server Error";
	static const _tstring not_implemented =
	  "Not Implemented";
	static const _tstring bad_gateway =
	  "Bad Gateway";
	static const _tstring service_unavailable =
	  "Service Unavailable";

} //StatusStrings

CStatus::CStatus(const StatusCode & eStatus) :
		m_eStatus(eStatus)
{

}

const _tstring& CStatus::ToString(const StatusCode & StatusCode)
{
	  switch (StatusCode)
	  {
	  case CStatus::StatusCode::eOk:
	    return StatusStrings::ok;
	  case CStatus::StatusCode::eCreated:
	    return StatusStrings::created;
	  case CStatus::StatusCode::eAccepted:
	    return StatusStrings::accepted;
	  case CStatus::StatusCode::eNoContent:
	    return StatusStrings::no_content;
	  case CStatus::StatusCode::eMultipleChoices:
	    return StatusStrings::multiple_choices;
	  case CStatus::StatusCode::eMovedPermanently:
	    return StatusStrings::moved_permanently;
	  case CStatus::StatusCode::eMovedTemporarily:
	    return StatusStrings::moved_temporarily;
	  case CStatus::StatusCode::eNotModified:
	    return StatusStrings::not_modified;
	  case CStatus::StatusCode::eBadRequest:
	    return StatusStrings::bad_request;
	  case CStatus::StatusCode::eUnauthorized:
	    return StatusStrings::unauthorized;
	  case CStatus::StatusCode::eForbidden:
	    return StatusStrings::forbidden;
	  case CStatus::StatusCode::eNotFound:
	    return StatusStrings::not_found;
	  case CStatus::StatusCode::eInternalServerError:
	    return StatusStrings::internal_server_error;
	  case CStatus::StatusCode::eNotImplemented:
	    return StatusStrings::not_implemented;
	  case CStatus::StatusCode::eBadGateway:
	    return StatusStrings::bad_gateway;
	  case CStatus::StatusCode::eServiceUnavailable:
	    return StatusStrings::service_unavailable;
	  default:
	    return StatusStrings::internal_server_error;
	  }
}

const _tstring& CStatus::ToAnswer()
{
	std::ostringstream ossAnswer;
	ossAnswer << "HTTP/1.0 " << static_cast< int > (m_eStatus) << " " << ToString(m_eStatus);

	m_sAnswerStatus = ossAnswer.str();
	return m_sAnswerStatus;
}

void CStatus::Set(const StatusCode & eStatus)
{
	m_eStatus = eStatus;
}

const _tstring CStatus::ToHtml()
{
	std::ostringstream ossResult;

	ossResult
		<< "<html>"
		<< "<head><title>" << cmn::CStatus::ToString(m_eStatus) << "</title></head>"
		<< "<body><h1>" << ToAnswer() << "</h1></body>"
		<< "</html>";

	return ossResult.str();
}

const CStatus::StatusCode & CStatus::Get() const
{
	return m_eStatus;
}

    /* namespace cmn */
}

