/*
 * MailSender.h
 *
 *  Created on: Dec 15, 2011
 *      Author: root
 */

#ifndef MAILSENDER_H_
#define MAILSENDER_H_

#include "Types.h"

#include <boost/asio/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace cmn
{

//! Class for sending email messages
class CMailSender
{
private:

	//! Type of the socket
	typedef boost::asio::ssl::stream< boost::asio::ip::tcp::socket > 	TSocket;

public:
	CMailSender(const _tstring& sServer, const _tstring& sPort, const _tstring& sEmailSrvAddress, const _tstring& sPassword);
	virtual ~CMailSender();

	//! Send message
	void 					Send(const _tstring& sDestination, const _tstring& sMessage);

private:

	//! Read-write function
	void 					ReadWrite(TSocket& Socket, const _tstring& sRequest, _tstring& sReply);

	//! Server email address
	const _tstring			m_sServerMail;

	//! Server email password
	const _tstring			m_sPassword;

	//! SMTP server
	const _tstring			m_sSMTPServer;

	//! SMTP server port
	const _tstring			m_sSMTPPort;

};

} /* namespace cmn */
#endif /* MAILSENDER_H_ */
