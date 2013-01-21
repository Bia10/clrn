#include "Player.h"
#include "../client/ITable.h"
#include "../client/PokerStarsTable.h"
#include "Actions.h"
#include "Log.h"
#include "Modules.h"
#include "CombinationsCalculator.h"

#include "gtest/gtest.h"

#include <iostream>

#include <boost/assign.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/random_device.hpp>
#include <boost/lexical_cast.hpp>

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
	TestTable() : m_Button(0), m_Table(new ps::Table(m_Log)), m_MaxBet(0)
	{
		m_Log.Open("tests.log", Modules::Table, ILog::Level::Debug);

		Evaluator ev;
		for (int i = 0 ; i < ::std::tr1::get<0>(GetParam()); ++i)
		{
			Player player;
			player.Name(std::string("Player_") + boost::lexical_cast<std::string>(i));
			player.Stack(1000);

			Card::List cards(2);
			cards[0].FromEvalFormat(ev.GetRandomCard());
			cards[1].FromEvalFormat(ev.GetRandomCard());

			player.Cards(cards);

			m_Players.push_back(player);
			m_Table->PlayerCards(player.Name(), player.Cards());
		}

		m_Moves.resize(m_Players.size());
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

			if (m_Moves[playerIndex] == Player::State::Fold)
				continue;

			if (m_Moves[playerIndex] == Player::State::InPot)
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
					action = Action::Call; 
				else
				{
					m_MaxBet = g_SmallBlind * 2;
					action = Action::Bet;
				}

				amount = m_MaxBet; 
				m_Moves[playerIndex] = Player::State::InPot;
				player->Bet(m_MaxBet);
				break;
			case 2: 
				action = Action::Bet; 
				amount = m_MaxBet * 2; 
				m_MaxBet = amount; 
				player->Bet(m_MaxBet);

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
		Player* player = &m_Players[m_Button];
		player->Bet(g_SmallBlind);

		m_Table->PlayerAction(player->Name(), Action::SmallBlind, g_SmallBlind);

		player = &m_Players[m_Button + 1];
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
			m_FlopCards.back().FromEvalFormat(ev.GetRandomCard());
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
		catch (const NoPlayers& /*e*/)
		{
			
		}
	}


private:
	Log m_Log;
	Player::List m_Players;
	std::size_t m_Button;
	std::auto_ptr<ITable> m_Table;
	std::size_t m_MaxBet;
	Card::List m_FlopCards;
	std::vector<Player::State::Value> m_Moves;
};


TEST_P(TestTable, Messages)
{
	Do();	
}

INSTANTIATE_TEST_CASE_P
(
	Combined,
	TestTable,
	Combine
	(
		Range(2, 10),
		Range(1, 10)
	)
);