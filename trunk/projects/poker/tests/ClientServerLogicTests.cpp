#include "gtest/gtest.h"

#include "FileSystem.h"
#include "Cards.h"
#include "../clientlib/PokerStarsTable.h"
#include "../clientlib/ITableControl.h"
#include "Log.h"
#include "IConnection.h"
#include "../serverlib/IStatistics.h"
#include "../serverlib/DecisionMaker.h"
#include "../serverlib/SqliteStatistics.h"
#include "../serverlib/MongoStatistics.h"
#include "../neuro/DatabaseReader.h"
#include "Evaluator.h"
#include "../neuro/NeuroNetwork.h"
#include "Config.h"
#include "Utils.h"

#include <fstream>
#include <algorithm>
#include <iterator>
#include <iomanip>

#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/filesystem.hpp>

using namespace pcmn;
using namespace clnt;
using testing::Range;
using testing::Combine;

class FakeControl : public clnt::ITableControl
{
public:
    virtual void Fold() override
    {

    }
    virtual void CheckCall() override
    {

    }
    virtual void BetRaise(unsigned amount) override
    {

    }
};

class EmptyTestStatistics : public srv::IStatistics
{
public:
	EmptyTestStatistics(ILog& logger)
	{

	}
	virtual void Write(pcmn::TableContext::Data& data) {}
	virtual unsigned GetRanges(PlayerInfo::List& players) const override { return 0; }
    virtual pcmn::Player::Style::Value GetAverageStyle(const std::string& target, const std::string& opponent) const override {}
	virtual void GetEquities(PlayerInfo::List& players, unsigned) const override {}
    virtual void GetHands(PlayerInfo& player, unsigned street, unsigned count) const override {}

};

class TestReadOnlyStatistics : public srv::MongoStatistics
{
public:
	TestReadOnlyStatistics(ILog& logger) : srv::MongoStatistics(logger)
	{

	}


	virtual void Write(pcmn::TableContext::Data& data) override
	{

	}
};

unsigned g_Answers = 0;

class TestClient : public net::IConnection
{
	virtual void Send(const google::protobuf::Message& message) 
	{
		std::cout << "Answer number: " << g_Answers++ << " " << message.DebugString() << std::endl;
        ASSERT_FALSE(g_Answers > 1);
	}

	virtual void Receive(const Callback& callback, const google::protobuf::Message* message = 0) 
	{
		throw std::exception("The method or operation is not implemented.");
	}

};

class FakeDecisionMaker : public pcmn::ITableLogicCallback
{
    virtual void SendRequest(const net::Packet& packet, bool statistics) override
    {
        throw std::exception("The method or operation is not implemented.");
    }

    virtual void MakeDecision(const Player& player, const Player::Queue& activePlayers, const TableContext& context, const Player::Position::Value position) override
    {
        std::cout << "Decision for: " << player.Name() << std::endl;
    }

    virtual void WriteStatistics(TableContext::Data& data) override
    {
    }

};

class TestServer : public net::IConnection
{
public:
	TestServer()
		: m_Network(m_Log, cfg::NETWORK_FILE_NAME)
		, m_Statistics(m_Log)
        , m_Evaluator(0)
	{

	}

	virtual void Send(const google::protobuf::Message& message) 
	{
        //FakeDecisionMaker maker;
        g_Answers = 0;
        net::IConnection::Ptr connection(new TestClient());
        neuro::DatabaseReader net(m_Log, cfg::NETWORK_FILE_NAME);
        srv::DecisionMaker maker(m_Log, m_Evaluator, m_Statistics, net, *connection);
        TableLogic logic(m_Log, maker, m_Evaluator);
        logic.Parse(static_cast<const net::Packet&>(message));

		static int counter = 0;
		std::cout << "Requests: " << counter++ << std::endl;
        g_Answers = 0;
	}

	virtual void Receive(const Callback& callback, const google::protobuf::Message* message = 0) 
	{
	}

private:
	Log m_Log;
	TestReadOnlyStatistics m_Statistics;
    //EmptyTestStatistics m_Statistics;
	pcmn::Evaluator	m_Evaluator;
	neuro::DatabaseReader m_Network;
};


void RunTest()
{
    boost::filesystem::directory_iterator it("../../../poker/logs/");
    boost::filesystem::directory_iterator end;

    pcmn::Evaluator evaluator;

    for (; it != end; ++it)
    {
        if (boost::filesystem::is_directory(it->path()))
            continue;

        const std::string full = boost::filesystem::system_complete(it->path()).string();

        std::cout << "Processing: '" << full << "' ..." << std::endl;

        // read file data
        std::ifstream ifs(full.c_str(), std::ios_base::binary);
        ASSERT_TRUE(ifs.is_open()) << full;

        ifs >> std::noskipws;

        std::string content;
        std::copy(std::istream_iterator<char>(ifs), std::istream_iterator<char>(), std::back_inserter(content));

        Log log;
        net::IConnection::Ptr connection(new TestServer());

        clnt::ITableControl* ctrl = new FakeControl();
        ps::Table table(log, NULL, connection, evaluator, ctrl);
        tests::ParseData(content, table);
    }
}

TEST(Logic, ClientServerByLogs)
{
	pcmn::Player::ThisPlayer().Name("CLRN");
	RunTest();
}