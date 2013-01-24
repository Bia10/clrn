#include "Log.h"
#include "Modules.h"
#include "UDPHost.h"
#include "IConnection.h"
#include "packet.pb.h"
#include "Actions.h"
#include "Player.h"

#include "gtest/gtest.h"

#include <iostream>

#include <boost/assign.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

using namespace pcmn;
using testing::Range;
using testing::Combine;

Log logger;

net::UDPHost srv(logger, 1);
net::UDPHost clnt(logger, 1);

std::size_t g_Counter = 0;

void ReceiveFromClientCallback(const google::protobuf::Message& message, const net::IConnection::Ptr& connection)
{
	connection->Send(message);
	++g_Counter;
	if (g_Counter > 100)
	{
		srv.Stop();
		clnt.Stop();

		std::cout << g_Counter << " messages transfered" << std::endl;
	}
}

void ReceiveFromServerCallback(const google::protobuf::Message& message, const net::IConnection::Ptr& connection)
{
	connection->Send(message);
	++g_Counter;
}

void TestFunc()
{
	net::Packet packet;

	net::Packet::Table& table = *packet.mutable_info();

	net::Packet::Player& added = *table.add_players();
	added.set_bet(123);
	added.set_name("name");
	added.set_stack(321);
	table.set_button(1);

	net::Packet::Phase& phase = *packet.add_phases();
	net::Packet::Action& action = *phase.add_actions();
	action.set_amount(1321);
	action.set_id(pcmn::Action::Ante);
	action.set_player(0);

	srv.Receive(boost::bind(&ReceiveFromClientCallback, _1, _2), packet, 5000);
	const net::IConnection::Ptr connection = clnt.Connect("127.0.0.1", 5000);

	connection->Send(packet);
	connection->Receive(boost::bind(&ReceiveFromServerCallback, _1, connection));

	srv.Run();
}


TEST(SendReceive, Simple)
{
	TestFunc();	
}
