#include "Player.h"
#include "../clientlib/ITable.h"
#include "../clientlib/PokerStarsTable.h"
#include "Actions.h"
#include "Log.h"
#include "Modules.h"
#include "Evaluator.h"
#include "IConnection.h"
#include "UDPHost.h"
#include "Config.h"

#include "gtest/gtest.h"

#include <iostream>

#include <boost/assign.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/random_device.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

using namespace pcmn;
using namespace clnt;
using testing::Range;
using testing::Combine;

const std::size_t g_SmallBlind = 10;
boost::random::mt19937 g_Generator;  

class NoPlayers : std::exception
{

};

std::string GetRandomString(std::size_t size = 10)
{
	static const std::string chars(
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"1234567890");

	std::string result;
	result.resize(size + 1);

	static boost::random::random_device rng;
	boost::random::uniform_int_distribution<> distribution(0, chars.size() - 1);
	for(std::size_t i = 0; i < size; ++i) 
		result[i] = chars[distribution(rng)];

	return result;
}

class TestTable : public testing::TestWithParam<::std::tr1::tuple<int, int> >
{
public:
	TestTable() : m_Server(new net::UDPHost(m_Log, 1)), m_Button(0), m_MaxBet(0)
	{
		//m_Log.Open("tests.log", Modules::Table, ILog::Level::Debug);
		m_Sender = m_Server->Connect("127.0.0.1", cfg::DEFAULT_PORT);
		m_Table.reset(new ps::Table(m_Log, m_Sender));
		memset(m_DeadCards, 0, _countof(m_DeadCards));
		
		Evaluator ev;
		for (int i = 0 ; i < ::std::tr1::get<0>(GetParam()); ++i)
		{
			const Player::Ptr player = boost::make_shared<Player>(std::string("Player_") + boost::lexical_cast<std::string>(i), 10000000);

			Card::List cards(2);
			cards[0].FromEvalFormat(ev.GetRandomCard(m_DeadCards));
			cards[1].FromEvalFormat(ev.GetRandomCard(m_DeadCards));

			player->Cards(cards);

			m_Players.push_back(player);
			m_Table->PlayerCards(player->Name(), player->Cards());
		}

		m_Moves.resize(m_Players.size());
	}

	~TestTable()
	{
		m_Server->Stop();
		m_Server->Wait();
	}

	void ClearBets()
	{
		for (const Player::Ptr& p : m_Players)
			p->Bet(0);

		m_MaxBet = 0;
	}

	void NextButton()
	{
		++m_Button;
		if (m_Button >= m_Players.size())
			m_Button = 0;
	}

	void GameLoop(const std::size_t playerStart)
	{
		for (std::size_t playerIndex = playerStart ; ; playerIndex += 1)
		{
			if (playerIndex >= m_Players.size())
				playerIndex = 0;

			if (m_Moves[playerIndex] == Player::State::Fold)
				continue;

			if (m_Moves[playerIndex] == Player::State::InPot)
				break;

			Player* player = m_Players[playerIndex].get();

			Action::Value action = Action::Unknown;
			std::size_t amount = 0;

			boost::random::uniform_int_distribution<> distribution(0, 2);
			const int rnd = distribution(g_Generator);
			switch (rnd)
			{
			case 0: 
				if (player->Bet() == m_MaxBet)
				{
					action = Action::Check;
					m_Moves[playerIndex] = Player::State::InPot;
				}
				else
				{
					action = Action::Fold; 
					m_Moves[playerIndex] = Player::State::Fold;
				}
				break;
			case 1: 
				if (m_MaxBet)
				{
					action = Action::Call; 
					amount = m_MaxBet - player->Bet(); 
				}
				else
				{
					m_MaxBet = g_SmallBlind * 2;
					action = Action::Bet;
					amount = m_MaxBet;
				}
				
				m_Moves[playerIndex] = Player::State::InPot;
				player->Bet(amount);
				break;
			case 2: 
				action = Action::Bet; 
				amount = m_MaxBet * 2; 
				m_MaxBet = amount; 
				player->Bet(amount);

				for (Player::State::Value& state : m_Moves)
				{
					if (state != Player::State::Fold)
						state = Player::State::Waiting;
				}

				m_Moves[playerIndex] = Player::State::InPot;
				break;
			}

			m_Table->PlayerAction(player->Name(), action, amount);
		}

		const std::size_t playersLeft = std::count(m_Moves.begin(), m_Moves.end(), Player::State::InPot);
		if (playersLeft < 2)
			throw NoPlayers();

		for (Player::State::Value& state : m_Moves)
		{
			if (state != Player::State::Fold)
				state = Player::State::Waiting;
		}

		NextButton();
	}

	void Preflop()
	{
		m_Table->PlayersInfo(m_Players);
		Player::Ptr player = m_Players[m_Button];
		player->Bet(g_SmallBlind);

		m_Table->PlayerAction(player->Name(), Action::SmallBlind, g_SmallBlind);

		player = m_Players[m_Button + 1];
		player->Bet(g_SmallBlind * 2);

		m_MaxBet = g_SmallBlind * 2;
		GameLoop(m_Button + 2);
	}

	void DealCards(const std::size_t count)
	{
		Evaluator ev;
		for (std::size_t i = 0 ; i < count ; ++i)
		{
			m_FlopCards.push_back(Card());
			m_FlopCards.back().FromEvalFormat(ev.GetRandomCard(m_DeadCards));
		}

		m_Table->FlopCards(m_FlopCards);
	}

	void Do()
	{
		try
		{
			Preflop();
			ClearBets();
			DealCards(3);
			ClearBets();
			GameLoop(m_Button + 1);
			DealCards(1);
			ClearBets();
			GameLoop(m_Button + 1);
			DealCards(1);
			ClearBets();
			GameLoop(m_Button + 1); 
		}
		catch (const NoPlayers&)
		{
			
		}

		// send statistics
		m_Table->PlayersInfo(m_Players);
	}

	void DoPredefinedActions()
	{
		{
			m_Players = boost::assign::list_of
				(boost::make_shared<Player>("ttommi", 488))
				(boost::make_shared<Player>("tonycry75", 970))
				(boost::make_shared<Player>("Shaggs1981", 398))
				(boost::make_shared<Player>("sevenup_king", 308))
				(boost::make_shared<Player>("PaulV39", 378))
				(boost::make_shared<Player>("TRUKHANOFF", 576))
				(boost::make_shared<Player>("LE CHACAL53", 692))
				(boost::make_shared<Player>("fialka03", 666));


			m_Table->PlayersInfo(m_Players);
			m_Table->PlayerAction("TRUKHANOFF", Action::SmallBlind, 15);
			m_Table->PlayerAction("fialka03", Action::Raise, 60);
			m_Table->PlayerAction("ttommi", Action::Fold, 0);
			m_Table->PlayerAction("tonycry75", Action::Raise, 970);
			m_Table->PlayerAction("Shaggs1981", Action::Fold, 0);
			m_Table->PlayerAction("sevenup_king", Action::Fold, 0);
			m_Table->PlayerAction("PaulV39", Action::Fold, 0);
			m_Table->PlayerAction("TRUKHANOFF", Action::Fold, 0);
			m_Table->PlayerAction("LE CHACAL53", Action::Fold, 0);
			m_Table->PlayerAction("fialka03", Action::Fold, 0);
		}

		{
			m_Players = boost::assign::list_of
				(boost::make_shared<Player>("loboda1968", 40))
				(boost::make_shared<Player>("sevenup_king", 524))
				(boost::make_shared<Player>("anatoliw", 1270))
				(boost::make_shared<Player>("pittipopo", 441))
				(boost::make_shared<Player>("LECHACAL53", 190))
				(boost::make_shared<Player>("Kuksman", 1255))
				(boost::make_shared<Player>("=Foksimus1=", 752))
				;
			m_Table->PlayersInfo(m_Players);

			m_Table->PlayerAction("sevenup_king", Action::SmallBlind, 20);
			m_Table->PlayerAction("pittipopo", Action::Fold, 0);
			m_Table->PlayerAction("LECHACAL53", Action::Call, 40);
			m_Table->PlayerAction("Kuksman", Action::Fold, 0);
			m_Table->PlayerAction("=Foksimus1=", Action::Call, 40);
			m_Table->PlayerAction("loboda1968", Action::Call, 40);
			m_Table->PlayerAction("sevenup_king", Action::Call, 20);
			m_Table->PlayerAction("anatoliw", Action::Check, 0);
			{
				const Card::List flopCards = boost::assign::list_of
					(Card(Card::King, Suit::Diamonds))
					(Card(Card::Four, Suit::Spades))
					(Card(Card::Three, Suit::Diamonds))
					;
				m_Table->FlopCards(flopCards);
			}

			m_Table->PlayerAction("sevenup_king", Action::Check, 0);
			m_Table->PlayerAction("anatoliw", Action::Check, 0);
			m_Table->PlayerAction("LECHACAL53", Action::Check, 0);
			m_Table->PlayerAction("=Foksimus1=", Action::Check, 0);

			{
				const Card::List flopCards = boost::assign::list_of
					(Card(Card::King, Suit::Diamonds))
					(Card(Card::Four, Suit::Spades))
					(Card(Card::Three, Suit::Diamonds))
					(Card(Card::Nine, Suit::Spades))
					;
				m_Table->FlopCards(flopCards);
			}

			m_Table->PlayerAction("sevenup_king", Action::Check, 0);
			m_Table->PlayerAction("anatoliw", Action::Check, 0);
			m_Table->PlayerAction("LECHACAL53", Action::Check, 0);
			m_Table->PlayerAction("=Foksimus1=", Action::Check, 0);
			{
				const Card::List flopCards = boost::assign::list_of
					(Card(Card::King, Suit::Diamonds))
					(Card(Card::Four, Suit::Spades))
					(Card(Card::Three, Suit::Diamonds))
					(Card(Card::Nine, Suit::Spades))
					(Card(Card::Ten, Suit::Clubs))
					;
				m_Table->FlopCards(flopCards);
			}

			m_Table->PlayerAction("sevenup_king", Action::Check, 0);
			m_Table->PlayerAction("anatoliw", Action::Bet, 40);
			m_Table->PlayerAction("LECHACAL53", Action::Fold, 0);
			m_Table->PlayerAction("=Foksimus1=", Action::Fold, 0);
 			m_Table->PlayerAction("sevenup_king", Action::Fold, 0);
			m_Table->PlayerAction("anatoliw", Action::MoneyReturn, 40);
			m_Table->PlayerAction("loboda1968", Action::Rank, 7);

			m_Table->PlayersInfo(m_Players);
		}
	}

private:
	Log m_Log;
	net::IHost::Ptr m_Server;
	net::IConnection::Ptr m_Sender;
	Player::List m_Players;
	std::size_t m_Button;
	std::auto_ptr<ITable> m_Table;
	std::size_t m_MaxBet;
	Card::List m_FlopCards;
	std::vector<Player::State::Value> m_Moves;
	bool m_DeadCards[cfg::CARD_DECK_SIZE];
};


TEST_P(TestTable, PredefinedActions)
{
	DoPredefinedActions();
} 

/*
TEST_P(TestTable, Messages)
{
	Do();	 
}*/

INSTANTIATE_TEST_CASE_P
(
	Combined,
	TestTable,
	Combine
	(
		Range(9, 10),
		Range(0, 10)
	)
);