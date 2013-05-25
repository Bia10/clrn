#include "Player.h"
#include "ITable.h"
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
#include <boost/thread.hpp>

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
	TestTable() : m_Server(new net::UDPHost(m_Log, 2)), m_Button(0), m_MaxBet(0)
	{
		//m_Log.Open("tests.log", Modules::Table, ILog::Level::Debug);
		m_Sender = m_Server->Connect("127.0.0.1", cfg::DEFAULT_PORT);
        const static pcmn::Evaluator evaluator;
		m_Table.reset(new ps::Table(m_Log, NULL, m_Sender, evaluator, 0));
		memset(m_DeadCards, 0, _countof(m_DeadCards));

		Player::ThisPlayer().Name("CLRN");
		
		Evaluator ev;
		for (int i = 0 ; i < ::std::tr1::get<0>(GetParam()); ++i)
		{
			Player player(std::string("Player_") + boost::lexical_cast<std::string>(i), 10000000);
			 
			Card::List cards(2);
			cards[0].FromEvalFormat(ev.GetRandomCard(m_DeadCards));
			cards[1].FromEvalFormat(ev.GetRandomCard(m_DeadCards));

			player.Cards(cards);

			m_Players.push_back(player);
			m_Table->PlayerCards(player.Name(), player.Cards());
		}

		m_Moves.resize(m_Players.size());
	}

	~TestTable()
	{
		boost::this_thread::interruptible_wait(100);
		m_Server->Stop();
		m_Server->Wait();
	}

	void ClearBets()
	{
		for (Player& p : m_Players)
			p.Bet(0);

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

			if (m_Moves[playerIndex] == Player::State::Folded)
				continue;

			if (m_Moves[playerIndex] == Player::State::Waiting)
				break;

			Player* player = &m_Players[playerIndex];

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
					m_Moves[playerIndex] = Player::State::Waiting;
				}
				else
				{
					action = Action::Fold; 
					m_Moves[playerIndex] = Player::State::Folded;
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
				
				m_Moves[playerIndex] = Player::State::Waiting;
				player->Bet(amount);
				break;
			case 2: 
				action = Action::Bet; 
				amount = m_MaxBet * 2; 
				m_MaxBet = amount; 
				player->Bet(amount);

				for (Player::State::Value& state : m_Moves)
				{
					if (state != Player::State::Folded)
						state = Player::State::Called;
				}

				m_Moves[playerIndex] = Player::State::Waiting;
				break;
			}

			m_Table->PlayerAction(player->Name(), action, amount);
		}

		const std::size_t playersLeft = std::count(m_Moves.begin(), m_Moves.end(), Player::State::Waiting);
		if (playersLeft < 2)
			throw NoPlayers();

		for (Player::State::Value& state : m_Moves)
		{
			if (state != Player::State::Folded)
				state = Player::State::Called;
		}

		NextButton();
	}

	void Preflop()
	{
		m_Table->PlayersInfo(m_Players);
		Player& player = m_Players[m_Button];
		player.Bet(g_SmallBlind);

		m_Table->PlayerAction(player.Name(), Action::SmallBlind, g_SmallBlind);

		player = m_Players[m_Button + 1];
		player.Bet(g_SmallBlind * 2);

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
				(Player("deathfight58", 416))
				(Player("batispedro", 476))
				(Player("BOIRA52", 476))
				(Player("lili544", 496))
				(Player("Mr.Speedy66", 634))
				(Player("gogo7211", 496))
				(Player("Robertilio", 496))
				(Player("isaac1111", 496))
				(Player("521710", 496))
				;
			m_Table->PlayersInfo(m_Players);

			m_Table->PlayerAction("BOIRA52", Action::SmallBlind, 10);
			m_Table->PlayerAction("Mr.Speedy66", Action::Fold, 0);
			m_Table->PlayerAction("gogo7211", Action::Fold, 0);
			m_Table->PlayerAction("Robertilio", Action::Fold, 0);
			m_Table->PlayerAction("isaac1111", Action::Raise, 100);
			m_Table->PlayerAction("521710", Action::Fold, 0);
			m_Table->PlayerAction("deathfight58", Action::Fold, 0);
			m_Table->PlayerAction("batispedro", Action::Call, 100);
			m_Table->PlayerAction("BOIRA52", Action::Fold, 0);
			m_Table->PlayerAction("lili544", Action::Call, 80);
			{const Card::List flopCards = boost::assign::list_of
				(Card(Card::Four, Suit::Diamonds))
				(Card(Card::Nine, Suit::Spades))
				(Card(Card::Seven, Suit::Hearts))
				;
			m_Table->FlopCards(flopCards);}

			m_Table->PlayerAction("lili544", Action::Check, 0);
			m_Table->PlayerAction("isaac1111", Action::Bet, 200);
			m_Table->PlayerAction("batispedro", Action::Raise, 376);
			m_Table->PlayerAction("lili544", Action::Call, 376);
			m_Table->PlayerAction("isaac1111", Action::Call, 176);
			{const Card::List flopCards = boost::assign::list_of
				(Card(Card::Four, Suit::Diamonds))
				(Card(Card::Nine, Suit::Spades))
				(Card(Card::Seven, Suit::Hearts))
				(Card(Card::King, Suit::Spades))
				;
			m_Table->FlopCards(flopCards);}

			m_Table->PlayerAction("lili544", Action::Check, 0);
			m_Table->PlayerAction("isaac1111", Action::Check, 0);
			{const Card::List flopCards = boost::assign::list_of
				(Card(Card::Four, Suit::Diamonds))
				(Card(Card::Nine, Suit::Spades))
				(Card(Card::Seven, Suit::Hearts))
				(Card(Card::King, Suit::Spades))
				(Card(Card::Jack, Suit::Diamonds))
				;
			m_Table->FlopCards(flopCards);}

			m_Table->PlayerAction("lili544", Action::Check, 0);
			m_Table->PlayerAction("isaac1111", Action::Check, 0);
			m_Table->PlayerAction("batispedro", Action::Loose, 0);
			m_Table->PlayerAction("batispedro", Action::Rank, 9);

			m_Table->PlayersInfo(m_Players);
		}
		
		{
			m_Players = boost::assign::list_of
				(Player("ttommi", 488))
				(Player("tonycry75", 970))
				(Player("Shaggs1981", 398))
				(Player("sevenup_king", 308))
				(Player("PaulV39", 378))
				(Player("TRUKHANOFF", 576))
				(Player("LE CHACAL53", 692))
				(Player("fialka03", 666));


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
				(Player("ttommi", 488))
				(Player("tonycry75", 970))
				(Player("Shaggs1981", 398))
				(Player("sevenup_king", 308))
				(Player(Player::ThisPlayer().Name(), 378))
				(Player("TRUKHANOFF", 576))
				(Player("LE CHACAL53", 692))
				(Player("fialka03", 666));


			m_Table->PlayersInfo(m_Players);

			Card card1;
			card1.m_Suit = Suit::Diamonds;
			card1.m_Value = Card::Ace;

			Card card2;
			card2.m_Suit = Suit::Diamonds;
			card2.m_Value = Card::King;

			m_Table->BotCards(card1, card2);

			m_Table->PlayerAction("TRUKHANOFF", Action::SmallBlind, 15);
			m_Table->PlayerAction("fialka03", Action::Raise, 60);
			m_Table->PlayerAction("ttommi", Action::Fold, 0);
			m_Table->PlayerAction("tonycry75", Action::Raise, 970);
			m_Table->PlayerAction("Shaggs1981", Action::Fold, 0);
			m_Table->PlayerAction("sevenup_king", Action::Fold, 0);
			m_Table->PlayerAction(Player::ThisPlayer().Name(), Action::SecondsLeft, 8);
		}

		{
			m_Players = boost::assign::list_of
				(Player("loboda1968", 40))
				(Player("sevenup_king", 524))
				(Player("anatoliw", 1270))
				(Player("pittipopo", 441))
				(Player("LECHACAL53", 190))
				(Player("Kuksman", 1255))
				(Player("=Foksimus1=", 752))
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