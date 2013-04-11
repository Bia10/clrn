#include "ActionsParser.h"
#include "Exception.h"
#include "Logic.h"
#include "TableActionsQueue.h"
#include "TableContext.h"
#include "Modules.h"

#include <sstream>

#include <boost/make_shared.hpp>

namespace clnt
{

unsigned CURRENT_MODULE_ID = Modules::TableLogic;

//! Unique add
template<typename C, typename V>
void UniqueAdd(C& c, const V& value)
{
	const typename C::const_iterator it = std::find(c.begin(), c.end(), value);
	assert(it == c.end());
	c.push_back(value);
}


ActionsParser::ActionsParser(ILog& logger, Actions& actions, pcmn::Player::List& players)
	: m_Actions(actions)
	, m_Players(players)
	, m_Log(logger)
{

}

bool ActionsParser::Parse(const bool isNeedDecision, std::string& button)
{
    SCOPED_LOG(m_Log);

    LOG_TRACE("Is need decision: [%s]") % isNeedDecision;

	if (ParseByAnte(button))
		return true;

	return ParseByBlinds(button, isNeedDecision);
}

bool ActionsParser::ParseByAnte(std::string& button)
{
    SCOPED_LOG(m_Log);

	std::vector<std::string> players;

	for (const ITable::ActionDesc& action : m_Actions.front())
	{
		if (action.m_Value == pcmn::Action::Ante)
			UniqueAdd(players, action.m_Name);
	}

	if (players.empty())
		return false;

	m_Players.clear();

	for (const std::string& name : players)
		m_Players.push_back(boost::make_shared<pcmn::Player>(name, 0));

	LinkPlayers(m_Players);
	RemoveUselessActions();

	const ITable::Actions& actions = m_Actions.front();
	if (actions.empty())
		return false;

	if (actions.front().m_Value != pcmn::Action::SmallBlind)
		return false;

	const pcmn::Player::Ptr buttonPlayer = GetPlayer(actions.front().m_Name);

	if (!buttonPlayer)
		return false;

	if (m_Players.size() == 2)
		button = buttonPlayer->Name();
	else
	if (buttonPlayer->GetPrevious())
		button = buttonPlayer->GetPrevious()->Name();
	else
		return false;

	InsertBigBlind();

	return true;
}

void ActionsParser::LinkPlayers(pcmn::Player::List& players)
{
    SCOPED_LOG(m_Log);

    if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Trace))
    {
        std::ostringstream oss;
        for (const pcmn::Player::Ptr& player : m_Players)
            oss << player->Name() << " ";

        LOG_TRACE("Players: [%s]") % oss.str();
    }

	// make links
	players.front()->SetPrevious(players.back());
	players.back()->SetNext(players.front());

	for (unsigned p = 0; p < players.size() - 1; ++p)
	{
		players[p]->SetNext(players[p + 1]);
		players[p + 1]->SetPrevious(players[p]);
	}
}

void ActionsParser::RemoveUselessActions()
{
    SCOPED_LOG(m_Log);

	for (ITable::Actions& actions : m_Actions)
	{
		actions.erase
		(
			std::remove_if
			(
				actions.begin(), 
				actions.end(), 
				[](const ITable::ActionDesc& action) 
				{
					return action.m_Value > pcmn::Action::BigBlind || action.m_Value == pcmn::Action::ShowCards; 
				}
			),
			actions.end()
		);
	}
}

bool ActionsParser::ParseByBlinds(std::string& button, const bool isNeedDecision)
{
    SCOPED_LOG(m_Log);

	RemoveUselessActions();

	ITable::Actions& actions = m_Actions.front();

	if (actions.empty() || actions.front().m_Value != pcmn::Action::SmallBlind)
		return false;

	if (ParseOneAction(button, isNeedDecision))
		return true;

	std::vector<std::string> players;
	ParsePlayerListByActions(players, isNeedDecision);

	const bool match = ArePlayersMatches(players);
	
	if (!match) // failed to find some players from actions in the players list
	{
		m_Players.clear();
		for (const std::string& name : players)
			m_Players.push_back(boost::make_shared<pcmn::Player>(name, 0));
	}

	if (!match || !isNeedDecision)
		LinkPlayers(m_Players);

	if (m_Players.size() == 2)
		button = GetPlayer(actions.front().m_Name)->Name();
	else
		button = GetPlayer(actions.front().m_Name)->GetPrevious()->Name();

	InsertBigBlind();
	return true;
}

pcmn::Player::Ptr ActionsParser::GetPlayer(const std::string& name) 
{
    SCOPED_LOG(m_Log);

	const pcmn::Player::List::iterator it = std::find_if
	(
		m_Players.begin(),
		m_Players.end(),
		[&](const pcmn::Player::Ptr& player)
		{
			return player ? player->Name() == name : false;
		}
	);

	if (it != m_Players.end())
		return *it;

	return pcmn::Player::Ptr();
}

void ActionsParser::CreateUnknown()
{
    SCOPED_LOG(m_Log);

	assert(!GetPlayer("Unknown"));
	m_Players.push_back(boost::make_shared<pcmn::Player>("Unknown", 0));
}

void ActionsParser::InsertBigBlind()
{
    SCOPED_LOG(m_Log);

	ITable::Actions& actions = m_Actions.front();
	assert(actions.front().m_Value == pcmn::Action::SmallBlind);

	if (actions[1].m_Value != pcmn::Action::BigBlind)
	{
		const pcmn::Player::Ptr smallBlind = GetPlayer(actions.front().m_Name);
		assert(smallBlind);
		const pcmn::Player::Ptr bigBlind = smallBlind->GetNext();
		assert(bigBlind);
		actions.insert(actions.begin() + 1, ITable::ActionDesc(bigBlind->Name(), pcmn::Action::BigBlind, actions[0].m_Amount * 2));
	}
}

bool ActionsParser::ParseOneAction(std::string& button, bool isNeedDecision)
{
    SCOPED_LOG(m_Log);

	ITable::Actions& actions = m_Actions.front();

	if (actions.size() != 1)
		return false;
	
	assert(isNeedDecision);
	const pcmn::Player::Ptr smallBlind = GetPlayer(actions.front().m_Name);
	if (smallBlind)
	{
		// try to find out big blind
		const pcmn::Player::Ptr bigBlind = smallBlind->GetNext();
		if (bigBlind)
			actions.push_back(ITable::ActionDesc(bigBlind->Name(), pcmn::Action::BigBlind, actions.front().m_Amount * 2));
		else
		{
			m_Players.clear();
			m_Players.push_back(smallBlind);
			CreateUnknown();
			actions.push_back(ITable::ActionDesc(m_Players.back()->Name(), pcmn::Action::BigBlind, actions.front().m_Amount * 2));
			LinkPlayers(m_Players);
		}

		// try to find button
		const pcmn::Player::Ptr buttonPlayer = smallBlind->GetPrevious();
		if (!buttonPlayer)
			LinkPlayers(m_Players);

		if (m_Players.size() == 2)
			button = GetPlayer(actions.front().m_Name)->Name();
		else
			button = GetPlayer(actions.front().m_Name)->GetPrevious()->Name();
	}
	return true;	
}

void ActionsParser::ParsePlayerListByActions(std::vector<std::string>& players, bool isNeedDecision)
{
    SCOPED_LOG(m_Log);

	ITable::Actions& actions = m_Actions.front();

	unsigned i = 0;
	const bool isBigBlindExists = std::find_if
	(
		actions.begin(), 
		actions.end(), 
		[](const ITable::ActionDesc& action)
		{return action.m_Value == pcmn::Action::BigBlind;}
	) != actions.end();

    LOG_TRACE("Big blind existence: [%s]") % isBigBlindExists;

	for (; i < actions.size(); ++i)
	{
		const ITable::ActionDesc& action = actions[i];

		if (i == actions.size() - 1 || i && action.m_Name == actions.front().m_Name) // small blind or last action
		{
			if (isBigBlindExists)
				break;

			// end of circle, next player on the big blind
			if (i + 1 < actions.size())
			{
                LOG_TRACE("Insert player by next: [%s]") % actions[i + 1].m_Name;
				players.insert(players.begin() + 1, actions[i + 1].m_Name); // next action exists
				break;
			}
            else
            if (!isNeedDecision)
            {
                LOG_TRACE("Insert current player: [%s]") % actions[i].m_Name;
                players.insert(players.begin() + 1, actions[i].m_Name); // this player on BB
                break;
            }
			else
			{
				const pcmn::Player::Ptr smallBlind = GetPlayer(actions.front().m_Name);
				if (smallBlind)
				{
					const pcmn::Player::Ptr bigBlind = smallBlind->GetNext();
					if (bigBlind)
					{
                        LOG_TRACE("Insert player from get next: [%s]") % bigBlind->Name();
						players.insert(players.begin() + 1, bigBlind->Name());
						break;
					}
				}

				const pcmn::Player::Ptr thisPlayer = GetPlayer(action.m_Name);
				if (thisPlayer)
				{
					const pcmn::Player::Ptr prevPlayer = thisPlayer->GetPrevious();
					if (prevPlayer)
					{
                        LOG_TRACE("Insert player from get prev: [%s]") % prevPlayer->Name();
						players.insert(players.begin() + 1, prevPlayer->Name());
						break;
					}
				}

				// not found player on big blind
                LOG_TRACE("Insert unknown player");
				players.insert(players.begin() + 1, "Unknown");
				m_Players.clear(); // old players list are not valid
				break;
			}
		}

		if (std::find(players.begin(), players.end(), action.m_Name) == players.end())
			players.push_back(action.m_Name);
	}

	const std::string& lastName = actions[i].m_Name;
	if (std::find(players.begin(), players.end(), lastName) == players.end())
		players.push_back(lastName);

	if (isNeedDecision && std::find(players.begin(), players.end(), pcmn::Player::ThisPlayer().Name()) == players.end())
		players.push_back(pcmn::Player::ThisPlayer().Name());

    if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Trace))
    {
        std::ostringstream oss;
        for (const std::string& player : players)
            oss << player << " ";

        LOG_TRACE("Players: [%s]") % oss.str();
    }
}

bool ActionsParser::ArePlayersMatches(const std::vector<std::string>& players)
{
    SCOPED_LOG(m_Log);

	ITable::Actions& actions = m_Actions.front();

	for (std::size_t i = 0 ; i < players.size() - 1; ++i)
	{
		const std::string& current = players[i];
		const std::string& next = players[i + 1];

		const pcmn::Player::Ptr currentPlayer = GetPlayer(current);

		if (!currentPlayer)
        {
            LOG_TRACE("Failed to find current, current: [%s], next: [%s]") % current % next;
			return false;
        }

		const pcmn::Player::Ptr nextPlayer = currentPlayer->GetNext();
		if (!nextPlayer || nextPlayer->Name() != next)
        {
            LOG_TRACE("Failed to find next, current: [%s], next: [%s]") % current % next;
			return false;
        }
	}

	return true;
}

} // namespace clnt