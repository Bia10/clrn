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
		
		ParsePlayers();

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

	std::vector<int> FindActivePlayer(const net::Packet::Phase& phase)
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

			const std::vector<int> activePlayers = FindActivePlayer(phase);
			const std::vector<short> ranges(activePlayers.size(), pcmn::Evaluator::CARD_DECK_SIZE);

			result.push_back(m_Evaluator.GetEquity(first, second, m_Result.m_Flop, ranges));
		}
		return result;
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