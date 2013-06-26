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


void ParseCards(const std::string& text, Card::List& result)
{
	static const boost::regex cards("\\('(.+?)' of '(.+?)'\\)");
	boost::sregex_iterator it(text.begin(), text.end(), cards);		
	boost::sregex_iterator end;

	if (it != end)
	{
		while (it != end)
		{
			const std::string& card = (*it)[1];
			const std::string& suit = (*it)[2];
			result.push_back(Card::FromString(card, suit));
			++it;
		}
	}
}

void ParseData(const std::string& data, ITable& table)
{

	static const boost::regex line("\\[.*?\\]\\:(.*?)/\\*");

	boost::sregex_iterator itLine(data.begin(), data.end(), line);
	boost::sregex_iterator end;

	pcmn::Player::List players;
	unsigned count = 0;
	for (; itLine != end; ++itLine, ++count)
	{
		boost::sregex_iterator itNext = itLine;
		++itNext;
		if (itNext != end)
			++itNext;
		if (itNext != end)
		{
			const std::string& nextLine = (*itNext)[0];
			if (nextLine.find("SecondsLeft") != std::string::npos && nextLine.find("CLRN") != std::string::npos)
				++itNext;
		}

		const std::string& fullLine = (*itLine)[0];
		const std::string& lineText = (*itLine)[1];

		// info
		{
			static const boost::regex info(".*'(.+)'.*stack.*'(\\d+)'");
			boost::sregex_iterator it(lineText.begin(), lineText.end(), info);

			while (it != end)
			{
				const std::string& name = (*it)[1];
				const unsigned stack = boost::lexical_cast<unsigned>((*it)[2]);

				players.push_back(Player(name, stack));
				++it;
			}
		}

		// actions
		{
			static const boost::regex action(".*'(.+)'.*action.*'(.+)',.*?(('(\\d+)')|(cards.*'(\\S+)'))");
			boost::sregex_iterator it(lineText.begin(), lineText.end(), action);		

			while (it != end)
			{
				if (!players.empty())
				{
					table.PlayersInfo(players);
					players.clear();
				}

				const std::string& name = (*it)[1];
				const std::string& action = (*it)[2];
				const std::string& cards = (*it)[7];

				if (cards.empty())
				{
					const unsigned amount = boost::lexical_cast<unsigned>((*it)[5]);
                    const pcmn::Action::Value actionValue = Action::FromString(action);
                    if (actionValue == pcmn::Action::SmallBlind)
                        std::cout << "small blind" << std::endl;
					table.PlayerAction(name, actionValue, amount);
				}
				else
				if (cards.size() == 4)
				{
					Card::List cardsList;

					Card tmp(Card::FromString(cards[0]), static_cast<Suit::Value>(cards[1]));
					cardsList.push_back(tmp);

					tmp.m_Value = Card::FromString(cards[2]);
					tmp.m_Suit = static_cast<Suit::Value>(cards[3]);
					cardsList.push_back(tmp);

					table.PlayerCards(name, cardsList);
					table.PlayerAction(name, Action::ShowCards, 0);
				}
				
				++it;
			}
		}		

		// flop cards
		{
			static const boost::regex cards("Cards\\sreceived(.*?)");
			boost::sregex_iterator it(lineText.begin(), lineText.end(), cards);		

			if (it != end)
			{
				while (it != end)
				{
					Card::List cards;
					ParseCards(lineText, cards);
					table.FlopCards(cards);
					++it;
				}
			}
		}

		// player cards
		{
			static const boost::regex action("Player\\scards(.*?)");
			boost::sregex_iterator it(lineText.begin(), lineText.end(), action);		

			while (it != end)
			{
				Card::List cards;
				ParseCards(lineText, cards);
				table.BotCards(cards[0], cards[1]);
				++it;
			}
		}	
	}
}

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
        ParseData(content, table);
    }
}

TEST(Logic, ClientServerByLogs)
{
	pcmn::Player::ThisPlayer().Name("CLRN");
	RunTest();
}