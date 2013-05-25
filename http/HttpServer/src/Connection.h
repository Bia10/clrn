/*
 * Connection.h
 *
 *  Created on: Nov 5, 2011
 *      Author: root
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "Status.h"
#include "Factory.h"

#include <vector>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/scoped_ptr.hpp>

//! Forward declarations
namespace http
{
	class CHttpData;
}

namespace srv
{

class CConnection :
	private boost::noncopyable,
	public boost::enable_shared_from_this< CConnection >
{
private:
	//! Reply type
	typedef boost::scoped_ptr< http::CHttpData > 		TReplyPtr;

	//! Request type
	typedef boost::scoped_ptr< http::CHttpData > 		TRequestPtr;

public:

	//! Socket type
	typedef boost::asio::ip::tcp::socket 				TSocket;

	//! Pointer type
	typedef boost::shared_ptr< CConnection > 			TPointer;

	//! Constructors
	CConnection() = delete; //!< Not implemented
	CConnection(boost::asio::io_service& Service, cmn::TExecutorsFactory& ExecutorsFactory);
	~CConnection();

	//! Read request
	void 										StartRead();

	//! Get socket refference
	inline TSocket& 							GetSocket()				{ return m_Socket; }

private:

	//! Read handler
	void 										Readhandler(const boost::system::error_code& e, const std::size_t& nReaded);

	//! Write handler
	void 										Writehandler(const boost::system::error_code& e, const std::size_t& nWrited);

	//! Error handler
	void 										ErrorHandler(const TCHAR * szRequest, const TCHAR * szMessage, const cmn::CStatus::StatusCode& eCode);

	//! Read validation
	const bool									IsRequestValid(const boost::system::error_code& ecRead, const std::size_t& nReaded);

	//! Connection socket
	TSocket										m_Socket;

	//! Request
	TRequestPtr									m_pRequest;

	//! Reply
	TReplyPtr									m_pReply;

	//!Buffer size
	static const std::size_t					ms_nBufferSize;

	//! Refference to executors factory
	cmn::TExecutorsFactory&						m_ExecutorsFactory;
};

} /* namespace srv */
#endif /* CONNECTION_H_ */
