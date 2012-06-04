#include "stdafx.h"


boost::asio::io_service io_service;
boost::array<char, 512> recv_buf;
boost::asio::ip::udp::socket* g_pSocket = 0;
boost::asio::ip::udp::endpoint ep;

typedef std::map<std::string, boost::asio::ip::udp::endpoint> ClientsMap;
ClientsMap clients;

void StartReceive();
void ReceiveHandle(const boost::system::error_code e, const std::size_t size)
{
	StartReceive();

	if (e)
	{
		std::cout << e.message() << std::endl;
		return;
	}

	const std::string data(recv_buf.data(), size);
	
	std::cout << "Received: " << data << std::endl;

	if (!clients.count(data))
	{
		const std::string source = ep.address().to_string() + ":" + conv::cast<std::string>(ep.port());

		std::cout << "Registering client: " << data << " with ep: " << source << std::endl;
		clients[data] = ep;
	}

	if (clients.size() == 2)
	{
		// both registered
		ClientsMap::const_iterator first = clients.begin(); 
		ClientsMap::const_iterator second = first; 
		++second;

		{
			const std::string buffer = second->second.address().to_string() + " " + conv::cast<std::string>(second->second.port());
			//const std::size_t sended = g_pSocket->send_to(boost::asio::buffer(buffer), first->second);

			std::cout << "Sended: " << buffer << std::endl;
		}
		{
			const std::string buffer = first->second.address().to_string() + " " + conv::cast<std::string>(first->second.port());
			//const std::size_t sended = g_pSocket->send_to(boost::asio::buffer(buffer), second->second);

			std::cout << "Sended: " << buffer << std::endl;
		}

		clients.clear();
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

int _tmain(int /*argc*/, char* argv[])
{
	try
	{
		boost::asio::ip::udp::resolver resolver(io_service);
		boost::asio::ip::udp::resolver::query query(boost::asio::ip::udp::v4(), argv[1], argv[2]);
		ep = *resolver.resolve(query);

		g_pSocket = new boost::asio::ip::udp::socket(io_service, ep);

		const std::string binded = ep.address().to_string() + ":" + conv::cast<std::string>(ep.port());
		std::cout << "Listening at: " << binded << std::endl;

		StartReceive();

		io_service.run();
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}