/*
 * Downloader.cpp
 *
 *  Created on: Aug 20, 2011
 *      Author: clrn
 */

#include "Downloader.h"

const std::size_t CDownloader::ms_nBufferSize = 4096;

CDownloader::CDownloader() :
	m_ReadStream(m_IoService),
	m_pBuffer(new char[ms_nBufferSize])
{
	// TODO Auto-generated constructor stub

}

CDownloader::~CDownloader()
{
	// TODO Auto-generated destructor stub
}

void CDownloader::Open(const std::string& sURL)
{
	m_ReadStream.open(sURL);
}

void CDownloader::Read(std::string& sBuffer)
{
	sBuffer.clear();
	boost::system::error_code ec;
	while (!ec)
	{
		std::size_t nReaded = boost::asio::read(m_ReadStream, boost::asio::buffer(m_pBuffer.get(), ms_nBufferSize), boost::asio::transfer_all(), ec);
		sBuffer.append(m_pBuffer.get(), nReaded);
	}
}
