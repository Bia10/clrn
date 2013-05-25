/*
 * Downloader.h
 *
 *  Created on: Aug 20, 2011
 *      Author: clrn
 */

#ifndef DOWNLOADER_H_
#define DOWNLOADER_H_

#include <string>

#include <boost/asio.hpp>
#include <boost/scoped_array.hpp>

#include <urdl/read_stream.hpp>

class CDownloader
{
public:
	CDownloader();
	~CDownloader();

	//! Открыть URL на чтение
	void Open(const std::string& sURL);

	//! Получить страницу
	void Read(std::string& sBuffer);

private:

	//! Объект ввода-вывода
	boost::asio::io_service m_IoService;

	//! Поток чтения
	urdl::read_stream m_ReadStream;

	//! Буффер чтения
	boost::scoped_array< char > m_pBuffer;

	//! Размер буффера чтения
	static const std::size_t ms_nBufferSize;


};

#endif /* DOWNLOADER_H_ */
