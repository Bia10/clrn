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

namespace srv
{

class Parser::Impl
{
public:
	Impl(ILog& logger, const net::Packet& packet) : m_Packet(packet), m_Log(logger)
	{
		SCOPED_LOG(m_Log);
		m_Result.m_NextPlayer = 0;
	}

	bool Parse()
	{
		SCOPED_LOG(m_Log);

		ParseFlopCards();
		ParseActivePlayers();
		ParsePlayers();

		std::vector<pcmn::Player> activePlayers(m_Result.m_Players.size());
		pcmn::Player* previous = 0;
		for (std::size_t i = m_Packet.info().button(), counter = 0 ; counter < activePlayers.size(); ++i, ++counter)
		{
			if (i == m_Result.m_Players.size())
				i = 0;

			const Data::Player& player = m_Result.m_Players[i];

			activePlayers[i] = pcmn::Player(
				player.m_Name,
				m_Packet.info().players(i).stack(),
				(i + 1 < m_Result.m_Players.size()) ? &activePlayers[i + 1] : 0,
				i ? &activePlayers[i - 1] : 0
			);

			//pcmn::Player& current = activePlayers.back();

		}

		typedef std::deque<pcmn::Player*> PlayerQueue;

		for (int i = 0 ; i < m_Packet.phases_size(); ++i)
		{
			PlayerQueue playerQueue;
			pcmn::TableContext context;

			pcmn::PacketActions packetActions(m_Packet.phases(i));

			// init queue
			for (pcmn::Player& player : activePlayers)
				playerQueue.push_back(&player);

			// next after the button
			playerQueue.push_back(playerQueue.front());
			playerQueue.pop_front();

			while (!playerQueue.empty())
			{
				pcmn::Player& current = *playerQueue.front();
				playerQueue.pop_front();

				const pcmn::IActionsQueue::Event::Value result = current.Do(packetActions, context);

				if (result == pcmn::IActionsQueue::Event::Raise)
				{
					pcmn::Player* next = current.GetNext();
					while (next && next != &current)
						playerQueue.push_back(next);
				}
				else
				if (result == pcmn::IActionsQueue::Event::NeedDecition)
				{

				}
				else
				if (result == pcmn::IActionsQueue::Event::Fold)
				{
					activePlayers.erase(std::find(activePlayers.begin(), activePlayers.end(), current));
				}
			}
		}




		for (int i = 0 ; i < m_Packet.phases_size(); ++i)
		{
			const net::Packet::Phase& phase = m_Packet.phases(i);
			ParseStreet(phase, i);
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
			m_PlayersStacks[i] = player.stack() + player.bet();
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

	//! Parse round data
	void ParseStreet(const net::Packet::Phase& phase, const int street)
	{
		SCOPED_LOG(m_Log);

		std::size_t pot = 0;
		std::vector<std::size_t> playerBets(m_Result.m_Players.size());
		std::vector<bool> activePlayers(m_Result.m_Players.size(), false);

		if (street)
		{
			const std::vector<int>& activePlayersOnThisTreet = m_Result.m_ActivePlayersPerStreet[street - 1];
			for (int player : activePlayersOnThisTreet)
				activePlayers[player] = true;
		}
		else
		{
			std::fill(activePlayers.begin(), activePlayers.end(), true); // preflop. all players are active
		}

		std::size_t lastPlayerIndex = 0;
		std::size_t maxBet = 0;
		for (int i = 0 ; i < phase.actions_size(); ++i)
		{
			const net::Packet::Action action = phase.actions(i);
			const int player = action.player();

			lastPlayerIndex = player;

			Data::Action resultAction;
			resultAction.m_Action = action.id();
			resultAction.m_PlayerIndex = player;
			resultAction.m_Street = street;
			resultAction.m_PotAmount = pot ? static_cast<float>(action.amount()) / pot : 1;
			resultAction.m_StackAmount = static_cast<float>(action.amount()) / m_PlayersStacks[player];
			resultAction.m_Position = GetPlayerPosition(street, player);

			assert(resultAction.m_PotAmount >= 0);
			assert(resultAction.m_StackAmount >= 0);

			switch (static_cast<pcmn::Action::Value>(resultAction.m_Action))
			{
			case pcmn::Action::Fold: 
				activePlayers[player] = false;
				break;
			case pcmn::Action::Bet: 
			case pcmn::Action::Raise:
				{
					const std::size_t difference = action.amount() - playerBets[player];
					m_PlayersStacks[player] -= difference;
					pot += difference;
					playerBets[player] += action.amount();

					if (playerBets[player] > maxBet)
						maxBet = playerBets[player];
					activePlayers[player] = true;
				}
				break; 
			case pcmn::Action::Call: 
			case pcmn::Action::SmallBlind: 
			case pcmn::Action::BigBlind:
				pot += action.amount();
				playerBets[player] += action.amount();
				m_PlayersStacks[player] -= action.amount();
				if (action.amount() > maxBet)
					maxBet = action.amount();
				activePlayers[player] = true;
				break;
			}

			m_Result.m_Actions.push_back(resultAction);
		}

		for (std::size_t i = lastPlayerIndex + 1 ; i != lastPlayerIndex ; ++i)
		{
			if (i == m_Result.m_Players.size())
			{
				i = 0;
				continue;
			}

			if (activePlayers[i] && playerBets[i] < maxBet)
			{
				std::cout << phase.DebugString() << std::endl;
				m_Result.m_NextPlayer = i;
				return;
			}
		}

	}

	//! Get player position
	pcmn::Player::Position::Value GetPlayerPosition(const int street, const int player)
	{
		const int playersSize = street ? m_Result.m_ActivePlayersPerStreet[street - 1].size() : m_Result.m_Players.size();

		int step = playersSize / 3;
		if (!step)
			step = 1;

		if (player + 1 <= step)
			return pcmn::Player::Position::Early;
		else
		if (player >= playersSize - step)
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