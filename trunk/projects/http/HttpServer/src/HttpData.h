/*
 * HttpData.h
 *
 *  Created on: Nov 13, 2011
 *      Author: root
 */

#ifndef HTTPDATA_H_
#define HTTPDATA_H_

#include "Types.h"
#include "IHttpData.h"

#include <map>

namespace http
{

class CHttpData: public http::IHttpData
{
	//! Headers type
	typedef std::map< _tstring, _tstring > 				THeaders;

public:
	CHttpData();
	virtual ~CHttpData();

	//! Get header value
	//!
	//!\param[in] sName									Header name
	//!\return 											Pointer to zero-terminated string
	//!
	virtual const _tstring&								GetHeaderValue(const _tstring& sName) const;

	//! Get request method
	//!
	//!\return 											String with method
	//!
	virtual const _tstring&								GetRequestMethod() const;

	//! Get request path
	//!
	//!\return 											String with path
	//!
	virtual const _tstring&								GetRequestPath() const;

	//! Get request extension
	//!
	//!\return 											String with extension
	//!
	virtual const _tstring&								GetRequestExtension() const;

	//! Get request post data
	//!
	//!\return											Pointer to zero-terminated string
	//!
	virtual const TCHAR*								GetPostData() const;

	//! Get cookie value
	//!
	//!\param[in] szName								Cookie name, zero-terminated
	//!\return 											String with cookie value
	//!
	virtual const _tstring								GetCookieValue(const _tstring& sName) const;

	//! Get request params(data after '?' in request path)
	//!
	//!\return 											map with request params(name=value)
	//!
	virtual const TRequestParams&						GetRequestParams() const;

	//! Set cookie value
	//!
	//!\param[in] szName								Cookie name
	//!\param[in] szValue								Cookie value
	//!
	virtual void										SetCookieValue(const _tstring& sName, const _tstring& sValue);

	//! Set header
	//!
	//!\param[in] sName									Header name
	//!\param[in] sValue								Header value
	//!
	virtual void										SetHeader(const _tstring& sName, const _tstring& sValue);

	//! Append content
	//!
	//!\param[in] sContent								Pointer to content
	//!\param[in] nLength								Content size
	//!
	virtual void										AppendContent(const TCHAR * szContent, const std::size_t& nLength);

	//! Append content
	//!
	//!\param[in] sContent								String containing server reply
	//!
	virtual void										AppendContent (const _tstring& sContent);

	//! Append file content
	//!
	//!\param[in] 										File name
	//!
	virtual void										AppendFileName (const _tstring& sFileName);

	//! Set mime type
	//!
	//!\param[in] sMimeType								Mime type(example: "text/html")
	//!
	virtual void 										SetMimeType (const _tstring& szMimeType);

	//! Set status
	//!
	//!\param[in] sStatus								Reply status(default: HTTP/1.0 200 OK)
	//!
	virtual void 										SetStatus (const _tstring& szStatus);

	//! Get buffer refference
	virtual TAsioBuffers								GetReplyBuffers();

	//! Get buffer refference
	virtual TBuffer&									GetRawBuffer();

	//! Parse request
	virtual void 										Parse();

private:

	//! Parse request parameters
	void 												ParseParams(const _tstring& sParams);

	//! Data buffer
	TBuffer												m_vecBuffer;

	//! Data begin position in buffer
	std::size_t											m_nDataIndex;

	//! Reply status
	_tstring											m_sStatus;

	//! Request path
	_tstring											m_sPath;

	//! Request extension
	_tstring											m_sExtension;

	//! Request method
	_tstring											m_sMethod;

	//! Request params
	TRequestParams										m_mapParams;

	//! Headers
	THeaders											m_mapHeaders;

};

} /* namespace http */
#endif /* HTTPDATA_H_ */
