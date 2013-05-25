//============================================================================
// Name        : QuickTests.cpp
// Author      : CLRN
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

/*
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <iostream>

#include <algorithm>
#include <fstream>
#include <sstream>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/detail/throw_error.hpp>
#include <boost/current_function.hpp>

#include <boost/archive/iterators/istream_iterator.hpp>
#include <boost/archive/iterators/ostream_iterator.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>

#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <boost/format.hpp>

#include <openssl/md5.h>

typedef boost::archive::iterators::istream_iterator<char> boost_istream_iterator;
typedef boost::archive::iterators::ostream_iterator<char> boost_ostream_iterator;

typedef boost::archive::iterators::insert_linebreaks<
    boost::archive::iterators::base64_from_binary<
        boost::archive::iterators::transform_width<boost_istream_iterator, 6, 8>
    >, 76
> bin_to_base64;

typedef boost::archive::iterators::transform_width<
    boost::archive::iterators::binary_from_base64<
        boost::archive::iterators::remove_whitespace<boost_istream_iterator>
    >, 8, 6
> base64_to_bin;


const std::string ToBase64(const std::string& sInput)
{
	std::ostringstream ossResult;

	std::istringstream iss(sInput);
	std::copy(
			bin_to_base64(boost_istream_iterator(iss >> std::noskipws)),
			bin_to_base64(boost_istream_iterator()),
			boost_ostream_iterator(ossResult)
	);

	return ossResult.str();
}

const std::string FromBase64(const std::string& sInput)
{
	std::ostringstream ossResult;

	std::istringstream iss(sInput);
	std::copy(
			base64_to_bin(boost_istream_iterator(iss >> std::noskipws)),
			base64_to_bin(boost_istream_iterator()),
			boost_ostream_iterator(ossResult)
	);

	return ossResult.str();
}


enum { max_length = 8192 };
class client
{
public:
  client(boost::asio::io_service& io_service, boost::asio::ssl::context& context,
      boost::asio::ip::tcp::endpoint& endpoint)
    : socket_(io_service, context)
  {
    //connect to the pop3 server
    socket_.lowest_layer().async_connect(endpoint,
        boost::bind(&client::handle_connect, this,
          boost::asio::placeholders::error));
  }
  ~client()
  {
     socket_.lowest_layer().close();
  }
  void handle_connect(const boost::system::error_code& error)
  {
    if (!error)
    {
      //do the SSL handshake
      socket_.async_handshake(boost::asio::ssl::stream_base::client,
          boost::bind(&client::handle_handshake, this,
            boost::asio::placeholders::error));
    }
    else
    {
      std::cerr << "Connection failed: " << error << "\n";
      boost::asio::detail::throw_error(error);
    }
  }
  void handle_handshake(const boost::system::error_code& error)
  {
    if (error)
    {
      std::cerr << "SSL Handshaking failed: " << error.message() << "\n";
      boost::asio::detail::throw_error(error);
    }
  }
  void write(std::string buffer)
  {
     str_req=buffer;
     boost::asio::async_write(socket_,
        boost::asio::buffer(str_req),
           boost::bind(&client::handle_write, this,
           boost::asio::placeholders::error,
           boost::asio::placeholders::bytes_transferred));
  }
  void handle_write(const boost::system::error_code& error,
      size_t bytes)
  {
    if (!error)
    {
      std::cout << "Sent REQ: " << str_req <<std::endl;
      try{
         boost::asio::async_read_until(socket_, buff_, "\r\n",
           boost::bind(&client::handle_read_until, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
      }catch(const boost::system::system_error &err){
         std::cerr << " code: " << err.code().value() << " :" << err.code().message() << std::endl;
      }
    }
    else
    {
      std::cerr << "Write failed: " << error.message() << "\n";
      boost::asio::detail::throw_error(error);
    }
  }
  void read()
  {
      try{
         boost::asio::async_read_until(socket_, buff_, "\r\n",
           boost::bind(&client::handle_read_until, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
      }catch(const boost::system::system_error &err){
        std::cerr << " code: " << err.code().value() << " :" << err.code().message() << std::endl;
      }
  }
  void handle_read_until(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      std::cout << "Got RES (" << bytes_transferred<<") bytes\n";
      std::cout << &buff_;
      std::cout << "\n";
    }
    else
    {
      std::cout << "Read failed: " << error.message() << "\n";
      boost::asio::detail::throw_error(error);
    }
  }
private:
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
  boost::asio::streambuf buff_; //for receiving
  std::string str_req; //for sending
};


#define WRITE(message) str_cmd = std::string(message) + "\r\n";  io_service.reset(); io_service.post(boost::bind(&client::write,&c,str_cmd)); io_service.run();


const std::string CalcMD5(const unsigned char * pData, const std::size_t& nLength)
{
	std::ostringstream ossResult;

	unsigned char pMD5[MD5_DIGEST_LENGTH];
	MD5(pData, nLength, pMD5);

	for (unsigned int i = 0; i < MD5_DIGEST_LENGTH; ++i)
	{
		ossResult << std::hex << static_cast< int > (pMD5[i]);
	}

	return ossResult.str();
}

/*
void ReadWrite(const _tstring& sMessage, _tstring& sResult)
{

}*/

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstring>

#include <boost/date_time.hpp>

const char TEXT[] = "asdfghjklpasdfghjklpasdfghjklpasdfghjklpasdfghjkl";

void TestCStyle()
{
	char pBuffer[500] = "";

	for (int i = 0; i < 10; ++i)
	{
		//strcpy(pBuffer + i * 50, TEXT);
		strcat(pBuffer, TEXT);
	}
}

std::ostringstream oss;
void TestStreamStyle()
{
	oss.str("");
	for (int i = 0; i < 10; ++i)
	{
		oss << TEXT;
	}
}

std::string sTest;
void TestStringStyle()
{
	sTest.clear();
	for (int i = 0; i < 10; ++i)
	{
		sTest.append(TEXT);
	}
}


typedef void (*TFunc)(void);

void TestFunc(const std::string& sWhat, TFunc pTestFunc)
{
	  boost::posix_time::ptime ptStart = boost::posix_time::microsec_clock::local_time();
	  for (int i = 0 ; i < 1000000; ++i)
	  pTestFunc();
	  boost::posix_time::time_duration td = boost::posix_time::microsec_clock::local_time() - ptStart;
	  std::cout << sWhat << " : " << td.total_milliseconds() << std::endl;
}


int main(int argc, char* argv[])
{
  try
  {
	  TestFunc("TestCStyle", 		&TestCStyle);
	  TestFunc("TestStreamStyle", 	&TestStreamStyle);
	  TestFunc("TestStringStyle", 	&TestStringStyle);

	  /*
	  std::string chars(
	          "abcdefghijklmnopqrstuvwxyz"
	          "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	          "1234567890");

	  boost::random::random_device rng;
	  boost::random::uniform_int_distribution<> index_dist(0, chars.size() - 1);

	  std::vector< unsigned char > vecRnd(200);

	  for(int i = 0; i < vecRnd.size(); ++i)
	  {
		  vecRnd[i] = chars[index_dist(rng)];
	  }
	  std::cout << &vecRnd.front() << std::endl;

	  std::cout << CalcMD5(&vecRnd.front(), vecRnd.size()) << std::endl;
	  /*
    if (argc != 5)
    {
      std::cerr << "Usage: " << argv[0] << " <host> <port> <user> <pass>\n";
      return 1;
    }*/
	 // smtp.gmail.com 25 clrnmail@gmail.com strenght1986
/*
    boost::asio::io_service io_service;
    boost::asio::ssl::context ctx(io_service, boost::asio::ssl::context::sslv23);
    ctx.set_verify_mode(boost::asio::ssl::context::verify_none);

    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query("smtp.gmail.com", "465");
    boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

    boost::asio::ip::tcp::endpoint endpoint = *iterator;

    boost::asio::ssl::stream< boost::asio::ip::tcp::socket > socket_(io_service, ctx);
    boost::asio::streambuf buff_; //for receiving
    std::string str_req; //for sending


    socket_.lowest_layer().connect(endpoint);
    socket_.handshake(boost::asio::ssl::stream_base::client);

    std::string sMessage = "HELO servername";
    socket_.write_some(boost::asio::buffer(sMessage));
    const std::size_t nReaded = boost::asio::read_until(socket_, buff_, "\r\n");

    boost::asio::streambuf::const_buffers_type bufs = buff_.data();

    const std::string sReply(
    		boost::asio::buffers_begin(bufs),
    		boost::asio::buffers_begin(bufs) + nReaded);

    std::cout << sReply << std::endl;

    client c(io_service, ctx, endpoint);
    io_service.run(); //should execute till handshake completes
    //read the pop3 greeting
    io_service.reset();
    io_service.post(boost::bind(&client::read,&c));
    io_service.run();


    std::string str_cmd;

    WRITE("HELO servername");
    WRITE("AUTH LOGIN");
    WRITE(ToBase64("clrnmail@gmail.com"));
    WRITE(ToBase64("strenght1986"));
    WRITE("MAIL FROM:<clrnmail@gmail.com>");
    WRITE("RCPT TO:<clrnmail@mail.ru>");
    WRITE("DATA\r\nSubject:  Hello from CLRN!");

    WRITE("SMTP test\r\n.");
    WRITE("QUIT");

    /*
    str_cmd="USER clrnmail@gmail.com\r\n";
    io_service.reset();
    io_service.post(boost::bind(&client::write,&c,str_cmd));
    io_service.run();

    str_cmd="PASS strenght1986\r\n";
    io_service.reset();
    io_service.post(boost::bind(&client::write,&c,str_cmd));
    io_service.run();



    //  io_service.reset();
  //  io_service.post(boost::bind(&client::write,&c,str_cmd));
  //  io_service.run();
    //get how many messages are in the inbox
    str_cmd="LIST\r\n";
    io_service.reset();
    io_service.post(boost::bind(&client::write,&c,str_cmd));
    io_service.run();

    str_cmd="RETR 1\r\n";
    io_service.reset();
    io_service.post(boost::bind(&client::write,&c,str_cmd));
    io_service.run();

    //quit the pop3 session
    str_cmd="QUIT\r\n";
    io_service.reset();
    io_service.post(boost::bind(&client::write,&c,str_cmd));
    io_service.run();*/
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
  return 0;
}

//
// client.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/*
#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

enum { max_length = 1024 };

class client
{
public:
  client(boost::asio::io_service& io_service, boost::asio::ssl::context& context,
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
    : socket_(io_service, context)
  {
    boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
    socket_.lowest_layer().async_connect(endpoint,
        boost::bind(&client::handle_connect, this,
          boost::asio::placeholders::error, ++endpoint_iterator));
  }

  void handle_connect(const boost::system::error_code& error,
      boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
  {
    if (!error)
    {
      socket_.async_handshake(boost::asio::ssl::stream_base::client,
          boost::bind(&client::handle_handshake, this,
            boost::asio::placeholders::error));
    }
    else if (endpoint_iterator != boost::asio::ip::tcp::resolver::iterator())
    {
      socket_.lowest_layer().close();
      boost::asio::ip::tcp::endpoint endpoint = *endpoint_iterator;
      socket_.lowest_layer().async_connect(endpoint,
          boost::bind(&client::handle_connect, this,
            boost::asio::placeholders::error, ++endpoint_iterator));
    }
    else
    {
      std::cout << "Connect failed: " << error << "\n";
    }
  }

  void handle_handshake(const boost::system::error_code& error)
  {
    if (!error)
    {
      std::cout << "Enter message: ";
      std::cin.getline(request_, max_length);
      size_t request_length = strlen(request_);

      boost::asio::async_write(socket_,
          boost::asio::buffer(request_, request_length),
          boost::bind(&client::handle_write, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      std::cout << "Handshake failed: " << error.message() << "\n";
    }
  }

  void handle_write(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      boost::asio::async_read(socket_,
          boost::asio::buffer(reply_, bytes_transferred),
          boost::bind(&client::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      std::cout << "Write failed: " << error << "\n";
    }
  }

  void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred)
  {
    if (!error)
    {
      std::cout << "Reply: ";
      std::cout.write(reply_, bytes_transferred);
      std::cout << "\n";
    }
    else
    {
      std::cout << "Read failed: " << error << "\n";
    }
  }

private:
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;
  char request_[max_length];
  char reply_[max_length];
};

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query("smtp.gmail.com", "587");
    boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

    boost::asio::ssl::context ctx(io_service, boost::asio::ssl::context::sslv23);
   // ctx.set_verify_mode(boost::asio::ssl::context::verify_peer);
  //  ctx.load_verify_file("/media/All/Dev/ThirdParty/boost_1_46_0/libs/asio/example/ssl/ca.pem");

    client c(io_service, ctx, iterator);

    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

*/


/*
#include <boost/asio.hpp>

#include <iostream>


void Write(boost::asio::ip::tcp::socket& Socket, const std::string& sMessage)
{
	boost::system::error_code error;
	Socket.write_some(boost::asio::buffer(sMessage.c_str(), sMessage.size()), error);
}

void Read(boost::asio::ip::tcp::socket& Socket)
{
	boost::system::error_code error;
	char Buffer[1024];
	std::size_t nReaded = Socket.read_some(boost::asio::buffer(Buffer), error);
	Buffer[nReaded] = 0;

	std::cout << Buffer << std::endl;
}

#define HELO "HELO 192.168.1.1\r\n"
#define DATA "DATA\r\n"
#define QUIT "QUIT\r\n"

char *host_id="192.168.1.10";
    char *from_id="clrnmail@gmail.com";
    char *to_id="clrnmail@gmail.com";
    char *sub="testmail\r\n";
    char wkstr[100]="hello how r u\r\n";

int main()
{
	boost::system::error_code error;

	boost::asio::io_service IO;

	boost::asio::ip::tcp::socket Socket(IO);

	boost::asio::ip::tcp::resolver Resolver(IO);

	boost::asio::ip::tcp::resolver::query Query("smtp.gmail.com", "mail");

	boost::asio::ip::tcp::resolver::iterator it = Resolver.resolve(Query);
	boost::asio::ip::tcp::resolver::iterator end;

    error = boost::asio::error::host_not_found;
    while (error && it != end)
    {
    	boost::asio::ip::tcp::endpoint Endpnt(*it);
		std::cout << Endpnt << std::endl;

		Socket.close();
		Socket.connect(*it++, error);
 		int nError = error.value();
		if (!error)
			break;
		std::string sError(error.message());
		std::cout << sError << " Error No :" << nError << std::endl;
	}
    if (error)
      throw boost::system::system_error(error);

	Socket.set_option(boost::asio::ip::tcp::socket::keep_alive(true));
	Socket.set_option(boost::asio::ip::tcp::socket::receive_buffer_size(1024));
	Socket.set_option(boost::asio::ip::tcp::socket::send_buffer_size(1024));

	char Buffer[1024];

	try
	{
		for (;;)
		{

		    Read(Socket); // SMTP Server logon string
		    Write(Socket, "HELO 195.161.101.33\r\n"); // introduce ourselves
		    Read(Socket); //Read reply
		    Write(Socket, "STARTTLS");
		    //Write(Socket, "clrnmail@gmail.com");
		    Write(Socket, "\r\n");
		    Read(Socket); //Sender OK
		    Write(Socket, "MAIL FROM:");
		    Write(Socket, "<clrnmail@gmail.com>");
		    Write(Socket, "\r\n");
		    Read(Socket); // Sender OK
		   // Write(Socket, "VRFY ");
		  //  Write(Socket, "<clrnmail@gmail.com>");
		  //  Write(Socket, "\r\n");
		 //   Read(Socket); // Sender OK
		    Write(Socket, "RCPT TO:"); //Mail to
		    Write(Socket, "<clrnmail@gmail.com>");
		    Write(Socket, "\r\n");
		    Read(Socket); // Recipient OK
		    Write(Socket, "DATA\r\n");// body to follow
		    Write(Socket, "Subject: ");
		    Write(Socket, sub);
		    Read(Socket); // Recipient OK
		    Write(Socket, wkstr);
		    Write(Socket, ".\r\n");
		    Read(Socket);
		    Write(Socket, QUIT); // quit
		    Read(Socket); // log off

			if (error == boost::asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw boost::system::system_error(error); // Some other error.
		}
    }
    catch (std::exception& e)
    {
		std::string sError(e.what());
		std::cerr << "Exception in thread: " << e.what() << "\n";
    }


	return 0;
}
*/
