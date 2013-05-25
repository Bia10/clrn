/*
 * IHttpData.h
 *
 *  Created on: Nov 13, 2011
 *      Author: root
 */

#ifndef IHTTPDATA_H_
#define IHTTPDATA_H_

#include "Types.h"

#include <vector>
#include <map>

//! Forward declarations
namespace srv
{
	class CConnection;
}

namespace boost
{
	namespace asio
	{
		class const_buffer;
	}
}

namespace http
{

//! Interface of the http request/reply
class IHttpData
{
public:

	//! Boost asio const buffer
	typedef boost::asio::const_buffer					TConstBuffer;

	//! Type of the boost::asio buffers
	typedef std::vector< TConstBuffer > 				TAsioBuffers;

	//! Buffer type
	typedef std::vector< TCHAR > 						TBuffer;

	//! Request params type
	typedef std::map< _tstring, _tstring > 				TRequestParams;

	IHttpData() {}
	virtual ~IHttpData() {}

	//! Get header value
	//!
	//!\param[in] sName									Header name
	//!\return 											Pointer to zero-terminated string
	//!
	virtual const _tstring&								GetHeaderValue(const _tstring& sName) const = 0;

	//! Get request method
	//!
	//!\return 											String with method
	//!
	virtual const _tstring&								GetRequestMethod() const = 0;

	//! Get request path
	//!
	//!\return 											String with path
	//!
	virtual const _tstring&								GetRequestPath() const = 0;

	//! Get request extension
	//!
	//!\return 											String with extension
	//!
	virtual const _tstring&								GetRequestExtension() const = 0;

	//! Get request post data
	//!
	//!\return											Pointer to zero-terminated string
	//!
	virtual const TCHAR*								GetPostData() const = 0;

	//! Get cookie value
	//!
	//!\param[in] szName								Cookie name, zero-terminated
	//!\return 											String with cookie value
	//!
	virtual const _tstring								GetCookieValue(const _tstring& sName) const = 0;

	//! Get request params(data after '?' in request path)
	//!
	//!\return 											map with request params(name=value)
	//!
	virtual const TRequestParams&						GetRequestParams() const = 0;

	//! Set cookie value
	//!
	//!\param[in] szName								Cookie name
	//!\param[in] szValue								Cookie value
	//!
	virtual void										SetCookieValue(const _tstring& sName, const _tstring& sValue) = 0;

	//! Set header
	//!
	//!\param[in] sName									Header name
	//!\param[in] sValue								Header value
	//!
	virtual void										SetHeader(const _tstring& sName, const _tstring& sValue) = 0;

	//! Append content
	//!
	//!\param[in] sContent								Pointer to content
	//!\param[in] nLength								Content size
	//!
	virtual void										AppendContent (const TCHAR * szContent, const std::size_t& nLength) = 0;

	//! Append content
	//!
	//!\param[in] sContent								String containing server reply
	//!
	virtual void										AppendContent (const _tstring& sContent) = 0;

	//! Append file content
	//!
	//!\param[in] 										File name
	//!
	virtual void										AppendFileName (const _tstring& sFileName) = 0;

	//! Set mime type
	//!
	//!\param[in] sMimeType								Mime type(example: "text/html")
	//!
	virtual void 										SetMimeType (const _tstring& szMimeType) = 0;

	//! Set status
	//!
	//!\param[in] sStatus								Reply status(default: HTTP/1.0 200 OK)
	//!
	virtual void 										SetStatus (const _tstring& szStatus) = 0;
};

} /* namespace http */

#endif /* IHTTPDATA_H_ */
