// UDPClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Conversion.h"

#include <iostream>
#include <string>
#include <map>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

boost::asio::io_service io_service;
boost::array<char, 512> recv_buf;
boost::asio::ip::udp::socket* g_pSocket = 0;
boost::asio::ip::udp::endpoint ep;

std::string clientId;

void StartReceive();
void ReceiveHandle(const boost::system::error_code e, const std::size_t size)
{
	StartReceive();

	if (e)
	{
		std::cout << e.value() << " " <<  e.message() << std::endl;
		return;
	}

	const std::string data(recv_buf.data(), size);
	
	std::cout << "Received: " << data << std::endl;

	std::vector<std::string> args;
	boost::algorithm::split(args, data, boost::algorithm::is_space());

	if (args.size() > 1)
	{
		boost::asio::ip::udp::resolver resolver(io_service);
		boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), args[0], args[1]);
		boost::asio::ip::udp::endpoint clientEp = *resolver.resolve(query);
		
		const std::string hello("ping!");
		g_pSocket->send_to(boost::asio::buffer(hello), clientEp);
		const std::string epStr = clientEp.address().to_string() + ":" + conv::cast<std::string>(clientEp.port());

		std::cout << "Sending ping! to: " << epStr << std::endl;
	}
	else
	{
		const std::string hello("pong!");
		g_pSocket->send_to(boost::asio::buffer(hello), ep);
		const std::string epStr = ep.address().to_string() + ":" + conv::cast<std::string>(ep.port());

		std::cout << "Sending pong! to: " << epStr << std::endl;
	}
}

void StartReceive()
{
	g_pSocket->async_receive_from
	(
		boost::asio::buffer(recv_buf), 
		ep,
		boost::bind
		(
			ReceiveHandle, 
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred
		)
	);
}

std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
	time_t now = time(0);

	std::vector<char> buffer(256);

	ctime_s(&buffer.front(), buffer.size(), &now);
	return std::string(buffer.begin(), buffer.end());
}

int _tmain(int /*argc*/, char* argv[])
{
	try
	{

		std::cout << "Server: " << argv[1] << " port: " << argv[2] << std::endl;
				
		boost::asio::ip::udp::resolver resolver(io_service);
		boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), argv[1], argv[2]);
		boost::asio::ip::udp::endpoint serverEndPoint = *resolver.resolve(query);
		const std::string srvEp = serverEndPoint.address().to_string() + ":" + conv::cast<std::string>(serverEndPoint.port());

		boost::asio::ip::udp::resolver::query localQuery(boost::asio::ip::udp::v4(), "127.0.0.1", "10000");
		boost::asio::ip::udp::endpoint localEndPoint = *resolver.resolve(localQuery);

		g_pSocket = new boost::asio::ip::udp::socket(io_service);
		g_pSocket->open(boost::asio::ip::udp::v4());
		clientId = make_daytime_string();

		for (;;)
		{
			/*const std::size_t sended = */g_pSocket->send_to(boost::asio::buffer(clientId), serverEndPoint);
			std::cout << "Sended: " << clientId << " to " << srvEp << std::endl;
		}

	//	StartReceive();

	//	io_service.run();
/*

		const std::string srvEp = serverEndPoint.address().to_string() + ":" + conv::cast<std::string>(serverEndPoint.port());

		Enum_t toDo = Connect;

		std::string host;
		std::string port;

		for (;;)
		{
			switch (toDo)
			{
			case Connect:
				{
					boost::asio::ip::udp::endpoint sender_endpoint;

					const std::size_t sended = g_pSocket->send_to(boost::asio::buffer(connectCmd), serverEndPoint);
					std::cout << "Sended: " << connectCmd << " to: " << srvEp << std::endl;	

					const std::size_t received = g_pSocket->receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

					const std::string reply(recv_buf.data(), received);
					std::cout << "Received: " << reply << " from: " << sender_endpoint.address().to_string() + ":" + conv::cast<std::string>(sender_endpoint.port()) << std::endl;	

					if (reply.empty())
					{
						std::cout << "Empty reply, registering..." << std::endl;
						toDo = Register;
						continue;
					}

					std::cout << "Got host: " << reply << " connecting..." << std::endl;

					const std::string::size_type delimiter = reply.find(':');

					if (std::string::npos == delimiter)
					{
						std::cout << "Bad host, connecting again..." << std::endl;
						toDo = Connect;
						continue;
					}

					host.assign(reply.substr(0, delimiter));
					port.assign(&reply[delimiter + 1]);

					toDo = Exchange;
					continue;
				}
				break;
			case Register:
				{
					boost::asio::ip::udp::endpoint sender_endpoint;

					const std::size_t sended = g_pSocket->send_to(boost::asio::buffer(registerCmd), serverEndPoint);
					std::cout << "Sended: " << registerCmd << " to: " << srvEp << std::endl;	
// 
// 					const std::size_t received = srvSocket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
// 
// 					const std::string reply(recv_buf.data(), received);
// 					std::cout << "Received: " << reply << " from: " << sender_endpoint.address().to_string() + ":" + conv::cast<std::string>(sender_endpoint.port()) << std::endl;	
// 
// 					if (reply.empty())
// 					{
// 						std::cout << "Bad reply, registering..." << std::endl;
// 						toDo = Register;
// 						continue;
// 					}

					toDo = WaitForData;
				}
				break;
			case Exchange:
				{
					boost::asio::ip::udp::resolver::query client(boost::asio::ip::udp::v4(), host, port);
					boost::asio::ip::udp::endpoint clientEp = *resolver.resolve(client);

					const std::string ep = clientEp.address().to_string() + ":" + conv::cast<std::string>(clientEp.port());

					boost::asio::ip::udp::endpoint sender_endpoint;

					for (;;)
					{	
						for (int i = 0; i < 50; ++i)
						{
							const std::size_t sended = g_pSocket->send_to(boost::asio::buffer(helloCmd), clientEp);
							std::cout << "Sended: " << helloCmd << " to: " << ep << std::endl;	
						}

						size_t len = g_pSocket->receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

						const std::string source = sender_endpoint.address().to_string() + ":" + conv::cast<std::string>(sender_endpoint.port());
						std::cout.write(recv_buf.data(), len);

						std::cout << "Received: " << std::string(&recv_buf.front(), len) << " from: " << source << std::endl;
					}
				}
				break;
			case WaitForData:
				{
					boost::asio::ip::udp::endpoint sender_endpoint;

					std::cout << "Waiting for connect... " << std::endl;

					for (;;)
					{	
						size_t len = g_pSocket->receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

						const std::string source = sender_endpoint.address().to_string() + ":" + conv::cast<std::string>(sender_endpoint.port());
						std::cout.write(recv_buf.data(), len);

						std::cout << "Received: " << std::string(&recv_buf.front(), len) << " from: " << source << std::endl;

						const std::size_t sended = g_pSocket->send_to(boost::asio::buffer(helloCmd), sender_endpoint);
						std::cout << "Sended: " << helloCmd << " to: " << source << std::endl;	
					}
				}
				break;
			}
		}

/ *

			std::size_t sended = srvSocket.send_to(boost::asio::buffer(connect), serverEndPoint);
			std::cout << "Sended: " << connect << " to: " << srvEp << std::endl;	

			std::size_t received = srvSocket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

			const std::string reply(recv_buf.data(), received);
			std::cout << "Received: " << reply << " from: " << sender_endpoint.address().to_string() + ":" + conv::cast<std::string>(sender_endpoint.port()) << std::endl;	

		}


			



		boost::array<char, 512> recv_buf;
		boost::asio::ip::udp::endpoint sender_endpoint;

		for (;;)
		{	

			size_t len = srvSocket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

			const std::string source = sender_endpoint.address().to_string() + ":" + conv::cast<std::string>(sender_endpoint.port());
			std::cout.write(recv_buf.data(), len);

			std::cout << "Received: " << std::string(&recv_buf.front(), len) << " from: " << source << std::endl;
		}* /*/
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}

