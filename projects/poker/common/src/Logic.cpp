#include "Logic.h"
#include "Modules.h"
#include "TableContext.h"
#include "Exception.h"

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

bool Logic::IsEmpty(const PlayerQueue& players) const
{
    const PlayerQueue::const_iterator it = std::find_if(players.begin(), players.end(), 
        [](const PlayerQueue::value_type& player)
        {
            return player->State() != pcmn::Player::State::AllIn;
        }
    );

    return it == players.end();
}

const Logic::PlayerQueue::value_type& Logic::GetLastActive(const PlayerQueue& players) const
{
    const std::reverse_iterator<PlayerQueue::const_iterator> it = std::find_if
    (
        players.rbegin(), 
        players.rend(), 
        [](const PlayerQueue::value_type& player)
        {
            return player->State() != pcmn::Player::State::AllIn;
        }
    );

    CHECK(it != players.rend(), "Failed to find last active player", players.size());
    return *it;
}

bool Logic::Run(TableContext& context)
{
	SCOPED_LOG(m_Log);

	TRY 
	{
		const std::size_t buttonIndex = m_Players.front()->Index();

		for (TableContext::Data::Player& player : context.m_Data.m_Players)
			player.m_TotalBet = 0;
	
		for (int street = 0 ; street < 4; ++street)
		{
			PlayerQueue playerQueue;
	
			// init queue
			for (const pcmn::Player::Ptr& player : m_Players)
				playerQueue.push_back(player);
	
			// next after the button
			if (playerQueue.front()->Index() == buttonIndex)
			{
				playerQueue.push_back(playerQueue.front());
				playerQueue.pop_front();
			}
	
			if (!street)
			{
				// add players on blinds
				playerQueue.push_back(playerQueue[0]);
				playerQueue.push_back(playerQueue[1]);

				if (m_Players.size() == 2)
				{
					playerQueue.push_back(playerQueue.front());
					playerQueue.pop_front();
				}
			}
	
			context.m_MaxBet = 0;
			for (const pcmn::Player::Ptr& player : m_Players)
				player->Bet(0);
	
			while (!IsEmpty(playerQueue))
			{
				const pcmn::Player::Ptr current = playerQueue.front();
                playerQueue.pop_front();

                if (current->State() == pcmn::Player::State::AllIn)
                {
                    if (current->GetNext() && current->GetPrevious())
                        current->DeleteLinks();
                    playerQueue.push_back(current); // move to the end
                    continue;
                }
	
				try
				{
                    if (playerQueue.empty())
                        return true;

					const unsigned previousBet = current->Bet();
					const pcmn::IActionsQueue::Event::Value result = current->Do(m_Actions, context);
					const Player::Position::Value position = GetPlayerPosition(m_Players, current);

					context.m_Data.m_Players[current->Index()].m_TotalBet += current->Bet() - previousBet;
	
					if (result == pcmn::IActionsQueue::Event::NeedDecision && !current->Cards().empty())
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
	
					current->PushAction(street, context.m_LastAction, resultAction.m_PotAmount);
					context.m_Data.m_Actions.push_back(resultAction);
	
					if (result == pcmn::IActionsQueue::Event::Raise)
					{
						pcmn::Player::Ptr next = IsEmpty(playerQueue) ? current->GetNext() : GetLastActive(playerQueue)->GetNext();
						while (next != current)
						{
                            if (next->State() != pcmn::Player::State::AllIn)
							    playerQueue.push_back(next);
	
							next = next->GetNext();
						}
					}
	
					if (result == pcmn::IActionsQueue::Event::Fold)
						EraseActivePlayer(playerQueue, current);
				}
				catch (const pcmn::Player::BadIndex& e)
				{
					const unsigned expected = e.Expected();
					const unsigned got = e.Got();
					const PlayerQueue::const_iterator itExpected = std::find_if(m_Players.begin(), m_Players.end(), 
						[&](const pcmn::Player::Ptr& player)
						{
							return player->Index() == expected;
						}
					);

					const PlayerQueue::const_iterator itGot = std::find_if(m_Players.begin(), m_Players.end(), 
						[&](const pcmn::Player::Ptr& player)
						{
							return player->Index() == got;
						}
					);

	
					CHECK(itExpected != m_Players.end(), "Failed to find expected player. Invalid query.", expected);
					CHECK(itGot != m_Players.end(), "Failed to find got player. Invalid query.", got);
					const std::string expectedPlayer = (*itExpected)->Name();
					const std::string gotPlayer = (*itGot)->Name();
	
					CHECK(false, "Invalid actions sequence detected", expectedPlayer, gotPlayer, expected, got, street);
				}
			}
	
			if (m_Players.size() < 2)
				break;
		}
	}
	CATCH_PASS_EXCEPTIONS("Failed to analyze table logic")

	return true;
}

//! Get player position
pcmn::Player::Position::Value Logic::GetPlayerPosition(const PlayerQueue& players, const pcmn::Player::Ptr& player)
{
	SCOPED_LOG(m_Log);

	const PlayerQueue::const_iterator it = std::find(players.begin(), players.end(), player);
	const std::size_t playerIndex = std::distance(players.begin(), it) - 1;

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
	CHECK(m_Players.size() >= 2, "Invalid players count, must be two at least", m_Players.size());

	for (std::size_t i = 0 ; i < m_Players.size(); ++i)
		m_PlayersIndexes.insert(std::make_pair(m_Players[i]->Name(), i));
}

void Logic::EraseActivePlayer(PlayerQueue& queue, const Player::Ptr& player)
{
	{
		const PlayerQueue::iterator it = std::find(m_Players.begin(), m_Players.end(), player);
		if (it == m_Players.end())
			return;

		(*it)->DeleteLinks();
		m_Players.erase(it);
	}

	const PlayerQueue::iterator it = std::find(queue.begin(), queue.end(), player);
	if (it == queue.end())
		return;

	queue.erase(it);
}

}