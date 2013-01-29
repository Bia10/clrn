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
		for (int i = 0 ; i < m_Packet.info().players_size(); ++i)
		{
			const net::Packet::Player& player = m_Packet.info().players(i);

			Data::Player p;
			p.m_Name = player.name();

			if (player.cards_size() == 2)
			{
				const int first = player.cards(0);
				const int second = player.cards(1);
			
				p.m_Percents = GetPlayerEquities(first, second);
			}

			m_Result.m_Players.push_back(p);
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

		return std::vector<int>();
	}

	//! Get player equity
	std::vector<float> GetPlayerEquities(const int first, const int second)
	{
		std::vector<float> result;
		for (int i = 0 ; i < m_Packet.phases_size(); ++i)
		{
			const net::Packet::Phase& phase = m_Packet.phases(i);

			const std::vector<int> activePlayers = FindActivePlayers(phase);
			const std::vector<short> ranges(activePlayers.size(), pcmn::Evaluator::CARD_DECK_SIZE);

			result.push_back(m_Evaluator.GetEquity(first, second, m_Result.m_Flop, ranges));
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

		const std::size_t step = players.size() / 3; // step per player position

		// set early stage for first players
		for (std::size_t i = 0 ; i < step + 1; ++i)
			m_Result.m_Players[i].m_Position = pcmn::Player::Position::Early;

		// set later position for last players
		for (std::size_t i = 0 ; i < step; ++i)
			m_Result.m_Players[m_Result.m_Players.size() - 1 - i].m_Position = pcmn::Player::Position::Later;

		// set middle position for least players
		for (std::size_t i = step + 1; i < m_Result.m_Players.size() - step; ++i)
			m_Result.m_Players[i].m_Position = pcmn::Player::Position::Middle;
	}

	//! Parse round data
	void ParseStreet()
	{

	}


private:

	//! Logger
	ILog& m_Log;

	//! Input packet
	const net::Packet& m_Packet;

	//! Result data
	Data m_Result;

	//! Evaluator
	pcmn::Evaluator m_Evaluator;
};


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