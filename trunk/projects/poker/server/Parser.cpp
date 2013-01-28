#include "Parser.h"
#include "Exception.h"
#include "CombinationsCalculator.h"

#include <iostream>

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

				std::vector<short> ranges(m_Packet.info().players_size(), pcmn::Evaluator::CARD_DECK_SIZE);
				//m_Evaluator.GetEquity(first, second, )
			}
			

			m_Result.m_Players.push_back(p);

		}
	}

	//! Get player equity
	void GetPlayerEquity(const int first, const int second)
	{

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