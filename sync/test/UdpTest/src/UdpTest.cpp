#include "stdafx.h"


CLog logger;
const std::string destination = "127.0.0.1:5000";

using boost::asio::ip::udp;

std::string make_daytime_string()
{
	using namespace std; // For time_t, time and ctime;
//	time_t now = time(0);
	return "time";//ctime(&now);
}


void SendLoop()
{
	const std::string message("hello!");
	for (;;)
	{
// 		TRY 
// 		{
// 			//net::CUDPSender::Instance().Send(destination, message.c_str(), message.size());
// 		}
// 		CATCH_IGNORE_EXCEPTIONS("SendLoop() failed.", logger)

	}
};

void RunStress()
{
	boost::thread_group threads;

	for (int i = 0; i < 8; ++i)
		threads.create_thread(&SendLoop);


	threads.join_all();
}

void RunSendPackets()
{
	for (;;)
	{
		TRY 
		{
			packets::Packet packet;
			packet.set_type(packets::Packet_PacketType_REQUEST);
/*
			jobs::Job* job = packet.add_jobs();

			job->set_id(jobs::Job_JobId_GET_SETTINGS);
	
			//net::CUDPSender::Instance().Send(destination, packet);

			boost::this_thread::sleep(boost::posix_time::seconds(5));*/
		}
		CATCH_PASS_EXCEPTIONS("RunSendPackets failed.")
	}
}

int _tmain(int /*argc*/, _TCHAR* argv[])
{
	std::cout << argv[1] << std::endl;

	for (;;)
	{
		try
		{

			boost::asio::io_service io_service;
			udp::resolver resolver(io_service);
			udp::resolver::query query(udp::v4(), argv[1], "5000");
			udp::endpoint receiver_endpoint = *resolver.resolve(query);
			udp::socket socket(io_service);
			socket.open(udp::v4());
			boost::array<char, 1> send_buf  = { 0 };
			socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);
			boost::array<char, 128> recv_buf;
			udp::endpoint sender_endpoint;
			size_t len = socket.receive_from(
				boost::asio::buffer(recv_buf), sender_endpoint);

			const std::string source = sender_endpoint.address().to_string() + ":" + conv::cast<std::string>(sender_endpoint.port());
			std::cout.write(recv_buf.data(), len);
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}


	}

//	logger.Open("1", ILog::Level::None);

	//net::CUDPSender::Create(logger);

//	RunSendPackets();
	//RunStress();

//	logger.Close();

//	return 0;
}


