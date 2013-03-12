#include "ActionsParser.h"
#include "Exception.h"

#include <boost/make_shared.hpp>

namespace clnt
{

//! Unique add
template<typename C, typename V>
void UniqueAdd(C& c, const V& value)
{
	const typename C::const_iterator it = std::find(c.begin(), c.end(), value);
	assert(it == c.end());
	c.push_back(value);
}


ActionsParser::ActionsParser(Actions& actions, pcmn::Player::List& players)
	: m_Actions(actions)
	, m_Players(players)
{

}

bool ActionsParser::Parse(const bool isNeedDecision, std::string& button)
{
	if (ParseByAnte(button))
		return true;

	return ParseByBlinds(button, isNeedDecision);
}

bool ActionsParser::ParseByAnte(std::string& button)
{
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

	LinkPlayers();
	RemoveUselessActions();

	const ITable::Actions& actions = m_Actions.front();
	assert(actions.front().m_Value == pcmn::Action::SmallBlind);

	if (m_Players.size() == 2)
		button = GetPlayer(actions.front().m_Name)->Name();
	else
		button = GetPlayer(actions.front().m_Name)->GetPrevious()->Name();

	InsertBigBlind();

	return true;
}

void ActionsParser::LinkPlayers()
{
	// make links
	m_Players.front()->SetPrevious(m_Players.back());
	m_Players.back()->SetNext(m_Players.front());

	for (unsigned p = 0; p < m_Players.size() - 1; ++p)
	{
		m_Players[p]->SetNext(m_Players[p + 1]);
		m_Players[p + 1]->SetPrevious(m_Players[p]);
	}
}

void ActionsParser::RemoveUselessActions()
{
	for (ITable::Actions& actions : m_Actions)
	{
		actions.erase
		(
			std::remove_if
			(
				actions.begin(), 
				actions.end(), 
				[](const ITable::ActionDesc& action) {return action.m_Value > pcmn::Action::BigBlind; }
			),
			actions.end()
		);
	}
}

bool ActionsParser::ParseByBlinds(std::string& button, const bool isNeedDecision)
{
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

		LinkPlayers();
	}

	if (m_Players.size() == 2)
		button = GetPlayer(actions.front().m_Name)->Name();
	else
		button = GetPlayer(actions.front().m_Name)->GetPrevious()->Name();

	InsertBigBlind();
	return true;
}

pcmn::Player::Ptr ActionsParser::GetPlayer(const std::string& name) 
{
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
	assert(!GetPlayer("Unknown"));
	m_Players.push_back(boost::make_shared<pcmn::Player>("Unknown", 0));
}

void ActionsParser::InsertBigBlind()
{
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
			LinkPlayers();
		}

		// try to find button
		const pcmn::Player::Ptr buttonPlayer = smallBlind->GetPrevious();
		if (!buttonPlayer)
			LinkPlayers();

		if (m_Players.size() == 2)
			button = GetPlayer(actions.front().m_Name)->Name();
		else
			button = GetPlayer(actions.front().m_Name)->GetPrevious()->Name();
	}
	return true;	
}

void ActionsParser::ParsePlayerListByActions(std::vector<std::string>& players, bool isNeedDecision)
{
	ITable::Actions& actions = m_Actions.front();

	unsigned i = 0;
	const bool isBigBlindExists = std::find_if
	(
		actions.begin(), 
		actions.end(), 
		[](const ITable::ActionDesc& action)
		{return action.m_Value == pcmn::Action::BigBlind;}
	) != actions.end();

	for (; i < actions.size(); ++i)
	{
		const ITable::ActionDesc& action = actions[i];

		if (i && action.m_Name == actions.front().m_Name || i == actions.size() - 1) // small blind or last action
		{
			if (isBigBlindExists)
				break;

			// end of circle, next player on the big blind
			if (i + 1 < actions.size())
			{
				players.insert(players.begin() + 1, actions[i + 1].m_Name); // next action exists
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
						players.insert(players.begin() + 1, bigBlind->Name());
						break;
					}
				}

				const pcmn::Player::Ptr thisPlayer = GetPlayer(action.m_Name);
				if (thisPlayer)
				{
					const pcmn::Player::Ptr nextPlayer = thisPlayer->GetNext();
					if (nextPlayer)
					{
						players.insert(players.begin() + 1, nextPlayer->Name());
						break;
					}
				}

				// not found player on big blind
				players.insert(players.begin() + 1, "Unknown");
				m_Players.clear(); // old players list are not valid
				break;
			}
		}

		players.push_back(action.m_Name);
	}

	const std::string& lastName = actions[i].m_Name;
	if (lastName != players.front())
		players.push_back(actions[i].m_Name);

	if (isNeedDecision)
		players.push_back(pcmn::Player::ThisPlayer().Name());
}

bool ActionsParser::ArePlayersMatches(const std::vector<std::string>& players)
{
	ITable::Actions& actions = m_Actions.front();

	for (std::size_t i = 0 ; i < players.size() - 1; ++i)
	{
		const std::string& current = players[i];
		const std::string& next = players[i + 1];

		const pcmn::Player::Ptr currentPlayer = GetPlayer(current);

		if (!currentPlayer)
			return false;

		const pcmn::Player::Ptr nextPlayer = currentPlayer->GetNext();
		if (!nextPlayer || nextPlayer->Name() != next)
			return false;
	}
	return true;
}

} // namespace clnt