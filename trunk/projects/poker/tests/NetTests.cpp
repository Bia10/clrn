#include "Player.h"
#include "../clientlib/ITable.h"
#include "../clientlib/PokerStarsTable.h"
#include "Actions.h"
#include "Log.h"
#include "Modules.h"
#include "CombinationsCalculator.h"
#include "IDataSender.h"
#include "UDPServer.h"
#include "../clientlib/DataSender.h"

#include "gtest/gtest.h"

#include <iostream>

#include <boost/assign.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/random_device.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

using namespace pcmn;
using namespace clnt;
using testing::Range;
using testing::Combine;

Log logger;
std::auto_ptr<net::UDPServer> srv;

//! Client handle
void HandleRequest(const net::UDPServer::BufferPtr& buffer, const net::UDPServer::IClient& client)
{
	srv->Stop();
}

TEST(SendReceive, Simple)
{
	srv.reset(new net::UDPServer(logger, 5000, 1, boost::bind(&HandleRequest, _1, _2)));

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
	action.set_player("name1");

	clnt::DataSender sender(logger);
	IDataSender& iface = sender;
	iface.OnGameFinished(packet);

	srv->Run();
}
