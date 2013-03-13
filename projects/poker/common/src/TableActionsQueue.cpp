#include "TableActionsQueue.h"

#include <algorithm>

namespace pcmn
{

TableActions::TableActions(const std::vector<clnt::ITable::Actions>& actions, const pcmn::Player::List& players) 
	: m_Index(0)
	, m_Actions(actions)
	, m_Street(0)
	, m_Players(players)
{

}

bool TableActions::Extract(Action::Value& action, unsigned& amount, unsigned& player) const 
{
	if (m_Actions.at(m_Street).empty())
		return false;

	if (m_Index >= m_Actions.at(m_Street).size())
	{
		m_Index = 0;
		++m_Street;
	}

	if (m_Actions.at(m_Street).empty())
		return false;

	action = m_Actions.at(m_Street).at(m_Index).m_Value;
	amount = m_Actions.at(m_Street).at(m_Index).m_Amount;

	const std::string name = m_Actions.at(m_Street).at(m_Index).m_Name;
	const pcmn::Player::List::const_iterator it = std::find_if
	(
		m_Players.begin(),
		m_Players.end(),
		[&](const pcmn::Player::Ptr& player)
		{
			return player->Name() == name;
		}
	);

	assert(it != m_Players.end());
	player = std::distance(m_Players.begin(), it);

	++m_Index;

	if (action > Action::BigBlind)
		return Extract(action, amount, player);

	return true;
}

}