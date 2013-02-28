#include "Parser.h"
#include "Exception.h"
#include "CombinationsCalculator.h"
#include "Actions.h"
#include "PacketActionsQueue.h"
#include "TableContext.h"

#include <iostream>
#include <set>
#include <queue>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/make_shared.hpp>

namespace srv
{

class Parser::Impl
{

	typedef std::deque<pcmn::Player::Ptr> PlayerQueue;

public:
	Impl(ILog& logger, const net::Packet& packet) : m_Packet(packet), m_Log(logger)
	{
		SCOPED_LOG(m_Log);
	}

	bool Parse()
	{
		SCOPED_LOG(m_Log);

		ParseFlopCards();
		ParseActivePlayers();
		ParsePlayers();

		PlayerQueue activePlayers(m_Result.m_Players.size());
		pcmn::Player::Ptr prev;
		for (std::size_t i = m_Packet.info().button(), counter = 0 ; ; ++i, ++counter)
		{
			if (i == m_Result.m_Players.size())
				i = 0;

			if (counter == activePlayers.size())
			{
				activePlayers[m_Packet.info().button()]->SetPrevious(prev);
				prev->SetNext(activePlayers[m_Packet.info().button()]);
				break;
			}

			const Data::Player& player = m_Result.m_Players[i];

			activePlayers[i] = boost::make_shared<pcmn::Player>(
				player.m_Name,
				m_Packet.info().players(i).stack()
			);

			if (prev)
			{
				activePlayers[i]->SetPrevious(prev);
				prev->SetNext(activePlayers[i]);
			}

			prev = activePlayers[i];
		}

		while (activePlayers.front()->Name() != m_Result.m_Players[m_Packet.info().button()].m_Name)
		{
			activePlayers.push_back(activePlayers.front());
			activePlayers.pop_front();
		}

		for (int i = 0 ; i < m_Packet.phases_size(); ++i)
		{
			PlayerQueue playerQueue;
			pcmn::TableContext context;

			pcmn::PacketActions packetActions(m_Packet.phases(i));

			// init queue
			for (const pcmn::Player::Ptr& player : activePlayers)
				playerQueue.push_back(player);

			// next after the button
			playerQueue.push_back(playerQueue.front());
			playerQueue.pop_front();

			while (!playerQueue.empty())
			{
				const pcmn::Player::Ptr current = playerQueue.front();
				playerQueue.pop_front();

				const pcmn::IActionsQueue::Event::Value result = current->Do(packetActions, context);

				if (result == pcmn::IActionsQueue::Event::NeedDecition)
				{
					return true;
				}

				Data::Action resultAction;
				resultAction.m_Action = context.m_LastAction;
				resultAction.m_PlayerIndex = m_PlayersIndexes[current->Name()];
				resultAction.m_Street = i;
				resultAction.m_PotAmount = context.m_Pot ? static_cast<float>(context.m_LastAmount) / context.m_Pot : 1;
				resultAction.m_StackAmount = static_cast<float>(context.m_LastAmount) / current->Stack();
				resultAction.m_Position = GetPlayerPosition(activePlayers, current);

				assert(resultAction.m_PotAmount >= 0);
				assert(resultAction.m_StackAmount >= 0);

				m_Result.m_Actions.push_back(resultAction);

				if (result == pcmn::IActionsQueue::Event::Raise)
				{
					pcmn::Player::Ptr next = playerQueue.back()->GetNext();
					while (next != current)
					{
						if (next->State() != pcmn::Player::State::AllIn)
							playerQueue.push_back(next);

						next = next->GetNext();
					}
				}
				else
				if (result == pcmn::IActionsQueue::Event::Fold)
				{
					const PlayerQueue::iterator it = std::find(activePlayers.begin(), activePlayers.end(), current);
					assert(it != activePlayers.end());
					(*it)->DeleteLinks();
					activePlayers.erase(it);
				}
			}
		}

		return false;
	}

	Data& GetResult()
	{
		return m_Result;
	}

private:

	//! ParseFlopCards
	void ParseFlopCards()
	{
		SCOPED_LOG(m_Log);

		for (int i = 0 ; i < m_Packet.info().cards_size(); ++i)
			m_Result.m_Flop.push_back(m_Packet.info().cards(i));
	}

	//! Parse players
	void ParsePlayers()
	{
		SCOPED_LOG(m_Log);

		m_PlayersStacks.resize(m_Packet.info().players_size());
		for (int i = 0 ; i < m_Packet.info().players_size(); ++i)
		{
			const net::Packet::Player& player = m_Packet.info().players(i);

			Data::Player p;
			p.m_Name = player.name();

			if (player.cards_size() == 2)
			{
				Data::Hand hand;
				hand.m_Cards.push_back(player.cards(0));
				hand.m_Cards.push_back(player.cards(1));
				hand.m_PlayerIndex = i;

				m_Result.m_Hands.push_back(hand);

				p.m_Percents = GetPlayerEquities(player.cards(0), player.cards(1));
			}

			m_Result.m_Players.push_back(p);
			m_PlayersStacks[i] = player.stack();
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

			const std::size_t activePlayers = i ? m_Result.m_ActivePlayersPerStreet[i - 1].size() : m_Result.m_Players.size();
			const std::vector<short> ranges(activePlayers, pcmn::Evaluator::CARD_DECK_SIZE);

			if (i == 1 && m_Result.m_Flop.size() >= 3)
				std::copy(m_Result.m_Flop.begin(), m_Result.m_Flop.begin() + 3, std::back_inserter(flop));
			else
			if (i && static_cast<int>(m_Result.m_Flop.size()) >= 2 + i)
				flop.push_back(m_Result.m_Flop[1 + i]);

			result.push_back(s_Evaluator.GetEquity(first, second, flop, ranges));
		}
		return result;
	}

	//! Get player position
	pcmn::Player::Position::Value GetPlayerPosition(const PlayerQueue& players, const pcmn::Player::Ptr& player)
	{
		const PlayerQueue::const_iterator it = std::find(players.begin(), players.end(), player);
		const std::size_t playerIndex = std::distance(players.begin(), it);

		std::size_t step = players.size() / 3;
		if (!step)
			step = 1;

		if (playerIndex <= step)
			return pcmn::Player::Position::Early;
		else
		if (playerIndex >= players.size() - step)
			return pcmn::Player::Position::Later;
		else
			return pcmn::Player::Position::Middle;
	}

	//! Parse player positions for street
	void ParseActivePlayers()
	{
		m_Result.m_ActivePlayersPerStreet.resize(m_Packet.phases_size());

		for (int p = 0 ; p < m_Packet.phases_size(); ++p)
		{
			const net::Packet::Phase& phase = m_Packet.phases(p);

			std::vector<int>& activePlayers = m_Result.m_ActivePlayersPerStreet[p];

			for (int i = 0 ; i < phase.actions_size(); ++i)
			{
				const net::Packet::Action action = phase.actions(i);

				switch (static_cast<pcmn::Action::Value>(action.id()))
				{
				case pcmn::Action::Fold: 
					{
						const std::vector<int>::const_iterator it = std::find(activePlayers.begin(), activePlayers.end(), action.player());
						if (it != activePlayers.end())
							activePlayers.erase(it);
					}

					break;
				case pcmn::Action::Bet: 
				case pcmn::Action::Raise:
				case pcmn::Action::Call: 
				case pcmn::Action::SmallBlind: 
				case pcmn::Action::BigBlind:
				case pcmn::Action::Check:
					if (std::find(activePlayers.begin(), activePlayers.end(), action.player()) == activePlayers.end())
						activePlayers.push_back(action.player());

					break;
				}
			}
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

	//! Player indexes map
	typedef std::map<std::string, std::size_t> PlayersMap;
	PlayersMap m_PlayersIndexes;

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

Parser::Data& Parser::GetResult() const
{
	return m_Impl->GetResult();
}



}