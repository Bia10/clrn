#ifndef Logic_h__
#define Logic_h__

#include "IActionsQueue.h"
#include "ILog.h"
#include "Player.h"

#include <deque>
#include <map>

#include <boost/noncopyable.hpp>

namespace pcmn
{

//! Decision callback
class IDecisionCallback
{
public:
	typedef std::deque<pcmn::Player::Ptr> PlayerQueue;

	virtual ~IDecisionCallback() {}

	virtual void MakeDecision
	(	
		const Player& player, 
		const PlayerQueue& activePlayers,
		const TableContext& context,
		const Player::Position::Value position
	) = 0;
};

//! Poker table logic implementation
class Logic
{
public:
	typedef std::deque<pcmn::Player::Ptr> PlayerQueue;
	typedef std::map<std::string, std::size_t> PlayersMap;

	Logic(ILog& logger, IActionsQueue& action, IDecisionCallback& callback, const PlayerQueue& players);

	bool Run(TableContext& context);

private:

	//! Parse input data
	void Parse();

	//! Erase active player
	void EraseActivePlayer(PlayerQueue& queue, const Player::Ptr& player);

	//! Get player position
	Player::Position::Value GetPlayerPosition(const PlayerQueue& players, const Player::Ptr& player);

    //! Is queue empty
    bool IsEmpty(const PlayerQueue& players) const;

    //! Get last active player
    const PlayerQueue::value_type& GetLastActive(const PlayerQueue& players) const;

private:

	//! Logger
	ILog& m_Log;

	//! Actions source
	IActionsQueue& m_Actions;

	//! Callback
	IDecisionCallback& m_Callback;

	//! Players
	PlayerQueue m_Players;

	//! Player indexes map
	PlayersMap m_PlayersIndexes;
};

}

#endif // Logic_h__
