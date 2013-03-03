#include "Logic.h"
#include "Modules.h"
#include "TableContext.h"

#include <cassert>

namespace pcmn
{

static const int CURRENT_MODULE_ID = Modules::TableLogic;


Logic::Logic(ILog& logger, IActionsQueue& action, IDecisionCallback& callback, const PlayerQueue& players)
	: m_Log(logger)
	, m_Actions(action)
	, m_Callback(callback)
	, m_Players(players)
{
	Parse();
}

bool Logic::Run(TableContext& context)
{
	SCOPED_LOG(m_Log);

	for (int street = 0 ; street < 4; ++street)
	{
		PlayerQueue playerQueue;

		// init queue
		for (const pcmn::Player::Ptr& player : m_Players)
			playerQueue.push_back(player);

		// next after the button
		playerQueue.push_back(playerQueue.front());
		playerQueue.pop_front();

		if (!street)
		{
			// add players on blinds
			playerQueue.push_back(playerQueue[0]);
			playerQueue.push_back(playerQueue[1]);
		}

		context.m_MaxBet = 0;

		while (!playerQueue.empty())
		{
			const pcmn::Player::Ptr current = playerQueue.front();
			playerQueue.pop_front();

			if (current->State() == pcmn::Player::State::AllIn)
			{
				EraseActivePlayer(current);
				continue;
			}

			const pcmn::IActionsQueue::Event::Value result = current->Do(m_Actions, context);
			const Player::Position::Value position = GetPlayerPosition(m_Players, current);

			if (result == pcmn::IActionsQueue::Event::NeedDecision)
			{
				m_Callback.MakeDecision(*current, m_Players, context, position);
				return false;
			}

			TableContext::Data::Action resultAction;
			resultAction.m_Action = context.m_LastAction;
			resultAction.m_PlayerIndex = m_PlayersIndexes[current->Name()];
			resultAction.m_Street = street;
			resultAction.m_PotAmount = context.m_Pot ? static_cast<float>(context.m_LastAmount) / context.m_Pot : 1;
			resultAction.m_StackAmount = current->Stack() ? static_cast<float>(context.m_LastAmount) / current->Stack() : 1;
			resultAction.m_Position = static_cast<int>(position);

			assert(resultAction.m_PotAmount >= 0);
			assert(resultAction.m_StackAmount >= 0);

			context.m_Data.m_Actions.push_back(resultAction);

			if (result == pcmn::IActionsQueue::Event::Raise)
			{
				pcmn::Player::Ptr next = playerQueue.empty() ? current->GetNext() : playerQueue.back()->GetNext();
				while (next != current)
				{
					playerQueue.push_back(next);

					next = next->GetNext();
				}
			}
			else
			if (result == pcmn::IActionsQueue::Event::Fold)
				EraseActivePlayer(current);
		}

		if (m_Players.size() < 2)
			break;
	}

	return true;
}

//! Get player position
pcmn::Player::Position::Value Logic::GetPlayerPosition(const PlayerQueue& players, const pcmn::Player::Ptr& player)
{
	SCOPED_LOG(m_Log);

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

void Logic::Parse()
{
	assert(m_Players.size() >= 2);

	for (std::size_t i = 0 ; i < m_Players.size(); ++i)
		m_PlayersIndexes.insert(std::make_pair(m_Players[i]->Name(), i));
}

void Logic::EraseActivePlayer(const Player::Ptr& player)
{
	const PlayerQueue::iterator it = std::find(m_Players.begin(), m_Players.end(), player);
	assert(it != m_Players.end());
	(*it)->DeleteLinks();
	m_Players.erase(it);
}

}