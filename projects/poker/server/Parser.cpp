#include "Parser.h"
#include "Exception.h"
#include "CombinationsCalculator.h"
#include "Actions.h"

#include <iostream>
#include <set>

#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace srv
{

class Parser::Impl
{
public:
	Impl(ILog& logger, const net::Packet& packet) : m_Packet(packet), m_Log(logger)
	{

	}

	bool Parse()
	{
		ParseFlopCards();
		ParsePlayers();
		ParsePlayersPositions();

		for (int i = 0 ; i < m_Packet.phases_size(); ++i)
		{
			const net::Packet::Phase& phase = m_Packet.phases(i);
			ParseStreet(phase, i);
		}

		return false;
	}

	const Data& GetResult() const
	{
		return m_Result;
	}

private:

	//! ParseFlopCards
	void ParseFlopCards()
	{
		for (int i = 0 ; i < m_Packet.info().cards_size(); ++i)
			m_Result.m_Flop.push_back(m_Packet.info().cards(i));
	}

	//! Parse players
	void ParsePlayers()
	{
		m_PlayersStacks.resize(m_Packet.info().players_size());
		for (int i = 0 ; i < m_Packet.info().players_size(); ++i)
		{
			const net::Packet::Player& player = m_Packet.info().players(i);

			Data::Player p;
			p.m_Name = player.name();

			if (player.cards_size() == 2)
			{
				Data::Hand hand;
				hand.m_First = player.cards(0);
				hand.m_Second = player.cards(1);
				hand.m_PlayerIndex = i;

				m_Result.m_Hands.push_back(hand);

				p.m_Percents = GetPlayerEquities(hand.m_First, hand.m_Second);
			}

			m_Result.m_Players.push_back(p);
			m_PlayersStacks[i] = player.stack() + player.bet();
		}
	}

	std::vector<int> FindActivePlayers(const net::Packet::Phase& phase)
	{
		std::set<int> result;
		for (int i = 0 ; i < phase.actions_size(); ++i)
		{
			const pcmn::Action::Value action = static_cast<pcmn::Action::Value>(phase.actions(i).id());
			if (pcmn::Action::IsActive(action))
				result.insert(phase.actions(i).player());
		}

		return std::vector<int>(result.begin(), result.end());
	}

	//! Get player equity
	std::vector<float> GetPlayerEquities(const int first, const int second)
	{
		std::vector<float> result;
		std::vector<int> flop;
		for (int i = 0 ; i < m_Packet.phases_size(); ++i)
		{
			const net::Packet::Phase& phase = m_Packet.phases(i);

			const std::vector<int> activePlayers = FindActivePlayers(phase);
			const std::vector<short> ranges(activePlayers.size(), pcmn::Evaluator::CARD_DECK_SIZE);

			if (i == 1 && m_Result.m_Flop.size() >= 3)
				std::copy(m_Result.m_Flop.begin(), m_Result.m_Flop.begin() + 3, std::back_inserter(flop));
			else
			if (i && static_cast<int>(m_Result.m_Flop.size()) >= 2 + i)
				flop.push_back(m_Result.m_Flop[1 + i]);

			result.push_back(s_Evaluator.GetEquity(first, second, flop, ranges));
		}
		return result;
	}

	//! Parse positions
	void ParsePlayersPositions()
	{
		std::vector<std::size_t> players;

		const int playerOnButtonIndex = m_Packet.info().button();
		const std::string& nameOnButton = m_Packet.info().players(playerOnButtonIndex).name();

		players.push_back(playerOnButtonIndex);
		for (std::size_t i = playerOnButtonIndex + 1; ; ++i)
		{
			if (i == m_Result.m_Players.size())
				i = 0;

			if (i == playerOnButtonIndex)
				break;

			players.push_back(i);
		}

		m_PlayersPositions.resize(players.size());

		const std::size_t step = players.size() / 3; // step per player position

		// set early stage for first players
		for (std::size_t i = 0 ; i < step + 1; ++i)
			m_PlayersPositions[i] = pcmn::Player::Position::Early;

		// set later position for last players
		for (std::size_t i = 0 ; i < step; ++i)
			m_PlayersPositions[m_Result.m_Players.size() - 1 - i] = pcmn::Player::Position::Later;

		// set middle position for least players
		for (std::size_t i = step + 1; i < m_Result.m_Players.size() - step; ++i)
			m_PlayersPositions[i] = pcmn::Player::Position::Middle;
	}

	//! Parse round data
	void ParseStreet(const net::Packet::Phase& phase, const int street)
	{
		std::size_t pot = 0;
		std::vector<int> playerBets(m_Result.m_Players.size());
		for (int i = 0 ; i < phase.actions_size(); ++i)
		{
			const net::Packet::Action action = phase.actions(i);
			const int player = action.player();

			Data::Action resultAction;
			resultAction.m_Action = action.id();
			resultAction.m_PlayerIndex = player;
			resultAction.m_Street = street;
			resultAction.m_PotAmount = pot ? static_cast<float>(action.amount()) / pot : 1;
			resultAction.m_StackAmount = static_cast<float>(action.amount()) / m_PlayersStacks[player];
			resultAction.m_Position = m_PlayersPositions[player];

			switch (static_cast<pcmn::Action::Value>(resultAction.m_Action))
			{
			case pcmn::Action::Bet: 
			case pcmn::Action::Raise:
				{
					const std::size_t difference = action.amount() - playerBets[player];
					m_PlayersStacks[player] -= difference;
					pot += difference;
					playerBets[player] += action.amount();
				}
				break;
			case pcmn::Action::Call: 
			case pcmn::Action::SmallBlind: 
			case pcmn::Action::BigBlind:
				pot += action.amount();
				playerBets[player] += action.amount();
				m_PlayersStacks[player] -= action.amount();
				break;
			}

			m_Result.m_Actions.push_back(resultAction);
		}
	}


private:

	//! Logger
	ILog& m_Log;

	//! Input packet
	const net::Packet& m_Packet;

	//! Result data
	Data m_Result;

	//! Evaluator
	static pcmn::Evaluator s_Evaluator;

	//! Player stacks
	std::vector<int> m_PlayersStacks;

	//! Players positions
	std::vector<pcmn::Player::Position::Value> m_PlayersPositions;
};

pcmn::Evaluator Parser::Impl::s_Evaluator;


Parser::Parser(ILog& logger, const net::Packet& packet) : m_Impl(new Impl(logger, packet))
{

}

Parser::~Parser()
{
	delete m_Impl;
}

bool Parser::Parse()
{
	return m_Impl->Parse();
}

const Parser::Data& Parser::GetResult() const
{
	return m_Impl->GetResult();
}



}