#include "Parser.h"
#include "Exception.h"
#include "Evaluator.h"
#include "Actions.h"
#include "PacketActionsQueue.h"
#include "TableContext.h"
#include "Config.h"
#include "Logic.h"

#include <iostream>
#include <set>
#include <queue>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <boost/assign.hpp>

namespace srv
{

const int CURRENT_MODULE_ID = Modules::Server;

class Parser::Impl : boost::noncopyable
{
public:
	Impl(ILog& logger, const pcmn::Evaluator& evaluator, const net::Packet& packet, pcmn::IDecisionCallback& callback) 
		: m_Packet(packet)
		, m_Log(logger)
		, m_Callback(callback)
		, m_Evaluator(evaluator)
	{
		SCOPED_LOG(m_Log);
	}

	bool Parse()
	{
		SCOPED_LOG(m_Log);

		ParseFlopCards();
		ParsePlayers();

		pcmn::Logic::PlayerQueue activePlayers(m_Context.m_Data.m_Players.size());
		pcmn::Player::Ptr prev;
		for (std::size_t i = m_Packet.info().button(), counter = 0 ; ; ++i, ++counter)
		{
			if (i == m_Context.m_Data.m_Players.size())
				i = 0;

			if (counter == activePlayers.size())
			{
				activePlayers[m_Packet.info().button()]->SetPrevious(prev);
				prev->SetNext(activePlayers[m_Packet.info().button()]);
				break;
			}

			const pcmn::TableContext::Data::Player& player = m_Context.m_Data.m_Players[i];

			activePlayers[i] = boost::make_shared<pcmn::Player>(
				player.m_Name,
				m_Packet.info().players(i).stack()
			);

			activePlayers[i]->Index(i);

			if (prev)
			{
				activePlayers[i]->SetPrevious(prev);
				prev->SetNext(activePlayers[i]);
			}

			prev = activePlayers[i];
		}

		for (const pcmn::TableContext::Data::Hand& hand : m_Context.m_Data.m_Hands)
		{
			const pcmn::Card::List cards = boost::assign::list_of(pcmn::Card().FromEvalFormat(hand.m_Cards[0]))(pcmn::Card().FromEvalFormat(hand.m_Cards[1]));
			activePlayers[hand.m_PlayerIndex]->Cards(cards);
		}

		while (activePlayers.front()->Name() != m_Context.m_Data.m_Players[m_Packet.info().button()].m_Name)
		{
			activePlayers.push_back(activePlayers.front());
			activePlayers.pop_front();
		}

		pcmn::PacketActions packetActions(m_Packet);

		pcmn::Logic logic(m_Log, packetActions, m_Callback, activePlayers);
		return logic.Run(m_Context);
	}

	pcmn::TableContext::Data& GetResult()
	{
		return m_Context.m_Data;
	}

private:

	//! ParseFlopCards
	void ParseFlopCards()
	{
		SCOPED_LOG(m_Log);

		for (int i = 0 ; i < m_Packet.info().cards_size(); ++i)
			m_Context.m_Data.m_Flop.push_back(m_Packet.info().cards(i));
	}

	//! Parse players
	void ParsePlayers()
	{
		SCOPED_LOG(m_Log);

		for (int i = 0 ; i < m_Packet.info().players_size(); ++i)
		{
			const net::Packet::Player& player = m_Packet.info().players(i);

			pcmn::TableContext::Data::Player p;
			p.m_Name = player.name();

			if (player.cards_size() == 2)
			{
				pcmn::TableContext::Data::Hand hand;
				hand.m_Cards.push_back(player.cards(0));
				hand.m_Cards.push_back(player.cards(1));
				hand.m_PlayerIndex = i;

				m_Context.m_Data.m_Hands.push_back(hand);

				p.m_Percents = GetPlayerEquities(player.cards(0), player.cards(1));
			}

			m_Context.m_Data.m_Players.push_back(p);
			CHECK(m_PlayersIndexes.insert(std::make_pair(player.name(), i)).second, "Bad request, player name must be unique", player.name());
		}
	}

	//! Get player equity
	std::vector<float> GetPlayerEquities(const int first, const int second)
	{
		SCOPED_LOG(m_Log);

		std::vector<float> result;
		std::vector<int> flop;
		for (int i = 0 ; i < m_Packet.phases_size(); ++i)
		{
			const net::Packet::Phase& phase = m_Packet.phases(i);

			const std::vector<short> ranges(cfg::MAX_EQUITY_PLAYERS, cfg::CARD_DECK_SIZE);

			if (i == 1 && m_Context.m_Data.m_Flop.size() >= 3)
				std::copy(m_Context.m_Data.m_Flop.begin(), m_Context.m_Data.m_Flop.begin() + 3, std::back_inserter(flop));
			else
			if (i && static_cast<int>(m_Context.m_Data.m_Flop.size()) >= 2 + i)
				flop.push_back(m_Context.m_Data.m_Flop[1 + i]);

			result.push_back(m_Evaluator.GetEquity(first, second, flop, ranges));
		}
		return result;
	}

private:

	//! Logger
	ILog& m_Log;

	//! Input packet
	const net::Packet& m_Packet;

	//! Result data
	pcmn::TableContext m_Context;

	//! Evaluator
	const pcmn::Evaluator& m_Evaluator;

	//! Player indexes map
	typedef std::map<std::string, std::size_t> PlayersMap;
	PlayersMap m_PlayersIndexes;

	//! Decision callback
	pcmn::IDecisionCallback& m_Callback;

};

Parser::Parser(ILog& logger, const pcmn::Evaluator& evaluator, const net::Packet& packet, pcmn::IDecisionCallback& callback) 
	: m_Impl(new Impl(logger, evaluator, packet, callback))
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

pcmn::TableContext::Data& Parser::GetResult() const
{
	return m_Impl->GetResult();
}



}