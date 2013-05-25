/*
 * Connection.cpp
 *
 *  Created on: Nov 5, 2011
 *      Author: root
 */

#include "Connection.h"
#include "Types.h"
#include "HttpData.h"
#include "Exception.h"
#include "ServerLogger.h"
#include "DefaultRequestExecutor.h"

#include <algorithm>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>


namespace srv
{

const std::size_t CConnection::ms_nBufferSize = 1024;

CConnection::CConnection(boost::asio::io_service& Service, cmn::TExecutorsFactory& ExecutorsFactory) :
		m_Socket(Service),
		m_pRequest(new http::CHttpData()),
		m_pReply(new http::CHttpData()),
		m_ExecutorsFactory(ExecutorsFactory)
{
	SCOPED_LOG_FUNCTION
}

CConnection::~CConnection()
{
	SCOPED_LOG_FUNCTION
}

void CConnection::StartRead()
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		// Reading request from socket
		const std::size_t nBeginData = m_pRequest->GetRawBuffer().size();
		m_pRequest->GetRawBuffer().resize(nBeginData + ms_nBufferSize);

		m_Socket.async_read_some(
				boost::asio::buffer(&m_pRequest->GetRawBuffer()[nBeginData], ms_nBufferSize),
				boost::bind(&CConnection::Readhandler,
						shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred
					)
		);
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());
}


const bool CConnection::IsRequestValid(const boost::system::error_code & ecRead, const std::size_t & nReaded)
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		if (!nReaded)
		{
			return false;
		}
		else
		if (!ecRead && ms_nBufferSize == nReaded)
		{
			StartRead();
			return false;
		}
		else
		if (ecRead && boost::asio::error::eof != ecRead)
		{
			THROW_EXCEPTION(ecRead.message().c_str());
		}

		m_pRequest->Parse();

		// Checking for valid size of recieved data, if Content-Length exists
		const _tstring& sContentLength = m_pRequest->GetHeaderValue(_T("Content-Length"));
		if (!sContentLength.empty())
		{
			const std::size_t nPostData = strlen(m_pRequest->GetPostData());
			const std::size_t nContentLength = boost::lexical_cast< std::size_t >(sContentLength);
			if (nPostData != nContentLength)
			{
				StartRead();
				return false;
			}
		}
	}
	CATCH_PASS_EXCEPTIONS_LOG(cmn::Logger::Instance());

	return true;
}

void CConnection::Readhandler(const boost::system::error_code & ecRead, const std::size_t& nReaded)
{
	SCOPED_LOG_FUNCTION

	const TCHAR * const pBuffer = &m_pRequest->GetRawBuffer().front();

	try
	{
		if (!IsRequestValid(ecRead, nReaded))
		{
			return;
		}

		// Getting executor object
		boost::scoped_ptr< IExecutor > pExecutor(m_ExecutorsFactory.Create(*m_pRequest));

		if (!pExecutor)
		{
			THROW_EXCEPTION_CODE(cmn::CStatus::StatusCode::eNotImplemented, "Failed to match executor to process request.");
		}

		if (cmn::Logger::Instance().IsEnabled())
		{
			cmn::Logger::Instance().Write(cmn::CFmtWrap(_T("Executing request with: [%s]"), cmn::ILog::Level::eDebug) % pExecutor->Description());
		}

		// Executing request
		pExecutor->Execute(*m_pRequest, *m_pReply);
	}
	catch(cmn::CException& e)
	{
		ErrorHandler(pBuffer, e.what(), e.code());
	}
	catch(std::exception& e)
	{
		ErrorHandler(pBuffer, e.what(), cmn::CStatus::StatusCode::eInternalServerError);
	}
	catch(...)
	{
		ErrorHandler(pBuffer, _T("Unhandled exception."), cmn::CStatus::StatusCode::eInternalServerError);
	}

	TRY_EXCEPTIONS
	{
		const http::IHttpData::TAsioBuffers& vecBuffers = m_pReply->GetReplyBuffers();

		boost::asio::async_write(
				m_Socket,
				vecBuffers,
				boost::bind(&CConnection::Writehandler,
						shared_from_this(),
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred
					)
		);
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

void CConnection::Writehandler(const boost::system::error_code & e, const std::size_t& nWrited)
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		// Closing connection
		boost::system::error_code ignored_ec;
		m_Socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	}
	CATCH_IGNORE_EXCEPTIONS_LOG(cmn::Logger::Instance());
}

void CConnection::ErrorHandler(const TCHAR *szRequest, const TCHAR *szMessage, const cmn::CStatus::StatusCode & eCode)
{
	SCOPED_LOG_FUNCTION

	TRY_EXCEPTIONS
	{
		cmn::CStatus Status(eCode);
		m_pReply->SetStatus(Status.ToAnswer().c_str());
		m_pReply->AppendContent(Status.ToHtml());
		m_pReply->AppendContent("\n");
		m_pReply->AppendContent(szMessage);
		const http::IHttpData::TAsioBuffers& vecBuffers = m_pReply->GetReplyBuffers();

		if (cmn::Logger::Instance().IsEnabled())
		{
			_tostringstream ossResult;
			for (const auto& Buffer: vecBuffers)
			{
				const _tstring& sBuffer = boost::asio::buffer_cast< const TCHAR * > (Buffer);
				ossResult << sBuffer;
			}

			cmn::Logger::Instance().Write(cmn::CFmtWrap(_T("Request execution failed: [%s]\nRequest: [%s]\nReply: [%s]"), cmn::ILog::Level::eError) % szMessage % szRequest % ossResult.str());
		}

	}
	CATCH_IGNORE_EXCEPTIONS_LOG(cmn::Logger::Instance());
}


} /* namespace srv */
