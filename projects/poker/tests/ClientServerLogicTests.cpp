#include "gtest/gtest.h"

#include "FileSystem.h"
#include "Cards.h"
#include "../clientlib/PokerStarsTable.h"
#include "Log.h"
#include "IConnection.h"
#include "../serverlib/IStatistics.h"
#include "../serverlib/DecisionMaker.h"
#include "../serverlib/Parser.h"
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

using namespace pcmn;
using namespace clnt;
using testing::Range;
using testing::Combine;


class TestStatistics : public srv::IStatistics
{
public:
	TestStatistics()
	{

	}
	virtual void Write(pcmn::TableContext::Data& data) 
	{
	}
};

class TestClient : public net::IConnection
{
	virtual void Send(const google::protobuf::Message& message) 
	{
		std::cout << message.DebugString() << std::endl;
	}

	virtual void Receive(const Callback& callback, const google::protobuf::Message* message = 0) 
	{
		throw std::exception("The method or operation is not implemented.");
	}

};

class TestServer : public net::IConnection
{
public:
	TestServer()
		: m_Network(cfg::NETWORK_FILE_NAME)
	{

	}

	virtual void Send(const google::protobuf::Message& message) 
	{
		TestClient client;
		srv::DecisionMaker decisionMaker(m_Log, m_Evaluator, m_Statistics, m_Network, client);

		srv::Parser parser(m_Log, m_Evaluator, dynamic_cast<const net::Packet&>(message), decisionMaker);

		if (parser.Parse())  // write statistics, game completed
			m_Statistics.Write(parser.GetResult());

		static int counter = 0;
		std::cout << "Requests: " << counter++ << std::endl;
	}

	virtual void Receive(const Callback& callback, const google::protobuf::Message* message = 0) 
	{
	}

private:
	TestStatistics m_Statistics;
	Log m_Log;
	pcmn::Evaluator	m_Evaluator;
	neuro::Network m_Network;
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

				players.push_back(boost::make_shared<Player>(name, stack));
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
					table.PlayerAction(name, Action::FromString(action), amount);
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
	static const std::string file = "messages.txt";
	const std::string full = fs::FullPath(file);

	// read file data
	std::ifstream ifs(full.c_str(), std::ios_base::binary);
	ASSERT_TRUE(ifs.is_open()) << full;

	ifs >> std::noskipws;

	std::string content;
	std::copy(std::istream_iterator<char>(ifs), std::istream_iterator<char>(), std::back_inserter(content));
	
	Log log;
	net::IConnection::Ptr connection(new TestServer());

	ps::Table table(log, NULL, connection);
	ParseData(content, table);
}

TEST(Logic, ClientServerByLogs)
{
	pcmn::Player::ThisPlayer().Name("CLRN");
	RunTest();
}