#ifndef TableActionsQueue_h__
#define TableActionsQueue_h__

#include "ITable.h"
#include "IActionsQueue.h"
#include "Player.h"

#include <vector>

namespace pcmn
{

class TableActions : public IActionsQueue
{
public:

	TableActions(const std::vector<clnt::ITable::Actions>& actions, const pcmn::Player::List& players);

	virtual bool Extract(Action::Value& action, unsigned& amount, unsigned& player) const override;

private:

	mutable unsigned m_Index;
	mutable unsigned m_Street;
	const std::vector<clnt::ITable::Actions>& m_Actions;
	const pcmn::Player::List& m_Players;
};

}
#endif // TableActionsQueue_h__
