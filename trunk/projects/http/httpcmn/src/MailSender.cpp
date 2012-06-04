/*
 * MailSender.cpp
 *
 *  Created on: Dec 15, 2011
 *      Author: root
 */

#include "MailSender.h"
#include "Exception.h"
#include "Converter.h"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/thread.hpp>

namespace cmn
{

CMailSender::CMailSender(const _tstring& sServer, const _tstring& sPort, const _tstring& sEmailSrvAddress, const _tstring& sPassword) :
		m_sServerMail(sEmailSrvAddress),
		m_sPassword(sPassword),
		m_sSMTPServer(sServer),
		m_sSMTPPort(sPort)
{

}

CMailSender::~CMailSender()
{

}

void CMailSender::Send(const _tstring & sDestination, const _tstring & sMessage)
{
	TRY_EXCEPTIONS
	{
		// boost::asio, ssl initialization
	    boost::asio::io_service IoService;
	    boost::asio::ssl::context SSLContext(IoService, boost::asio::ssl::context::sslv23);
	    SSLContext.set_verify_mode(boost::asio::ssl::context::verify_none);

	    // Resolving server
	    boost::asio::ip::tcp::resolver Resolver(IoService);
	    boost::asio::ip::tcp::resolver::query Query(m_sSMTPServer, m_sSMTPPort);
	    boost::asio::ip::tcp::resolver::iterator it = Resolver.resolve(Query);

	    // Connect endpoint
	    boost::asio::ip::tcp::endpoint Endpoint = *it;

	    TSocket Socket(IoService, SSLContext);

	    // Connecting
	    Socket.lowest_layer().connect(Endpoint);
	    Socket.handshake(boost::asio::ssl::stream_base::client);

	    // Starting read-write sequence
	    _tostringstream ossSendBuffer;
	    _tstring sAnswer;
	    ossSendBuffer << _T("HELO idontcare.com\r\n");
	    ReadWrite(Socket, ossSendBuffer.str(), sAnswer);

	    ossSendBuffer.str(_T(""));
	    ossSendBuffer << _T("AUTH LOGIN") << _T("\r\n");
	    ReadWrite(Socket, ossSendBuffer.str(), sAnswer);

	    ossSendBuffer.str(_T(""));
	    ossSendBuffer << CConverter::ToBase64(m_sServerMail) << _T("\r\n");
	    ReadWrite(Socket, ossSendBuffer.str(), sAnswer);

	    ossSendBuffer.str(_T(""));
	    ossSendBuffer << CConverter::ToBase64(m_sPassword) << _T("\r\n");
	    ReadWrite(Socket, ossSendBuffer.str(), sAnswer);

	    ossSendBuffer.str(_T(""));
	    ossSendBuffer << _T("MAIL FROM:<") << m_sServerMail << _T(">\r\n");
	    ReadWrite(Socket, ossSendBuffer.str(), sAnswer);

	    ossSendBuffer.str(_T(""));
	    ossSendBuffer << _T("RCPT TO:<") << sDestination << _T(">\r\n");
	    ReadWrite(Socket, ossSendBuffer.str(), sAnswer);

	    ossSendBuffer.str(_T(""));
	    ossSendBuffer << _T("DATA\r\nSubject: Http server message.\r\n");
	    ReadWrite(Socket, ossSendBuffer.str(), sAnswer);

	    ossSendBuffer.str(_T(""));
	    ossSendBuffer << sMessage << _T("\r\n.\r\n");
	    ReadWrite(Socket, ossSendBuffer.str(), sAnswer);

	    ossSendBuffer.str(_T(""));
	    ossSendBuffer << _T("QUIT") << _T("\r\n");
	    ReadWrite(Socket, ossSendBuffer.str(), sAnswer);
	}
	CATCH_PASS_EXCEPTIONS
}

void CMailSender::ReadWrite(TSocket& Socket, const _tstring & sRequest, _tstring & sReply)
{
	TRY_EXCEPTIONS
	{
		// Write request
		Socket.write_some(boost::asio::buffer(sRequest));

		boost::this_thread::sleep(boost::posix_time::milliseconds(200));

		if (sRequest == _T("QUIT\r\n"))
			return;

		// Buffer for receiving
		std::vector< TCHAR > vecReply(512);

		const std::size_t nReaded = Socket.read_some(boost::asio::buffer(vecReply));

	    sReply.assign(vecReply.begin(), vecReply.begin() + nReaded);

	    if (!sReply.empty() && sReply[0] != '5')
	    	return;

	    _tostringstream ossException;
	    ossException
	    	<< _T("Send email failed. Request : ") << sRequest << std::endl
	    	<< _T("Server answer: ") << sReply << std::endl;
	    THROW_EXCEPTION(ossException.str());
	}
	CATCH_PASS_EXCEPTIONS
}


} /* namespace cmn */
