#ifndef ActionsParser_h__
#define ActionsParser_h__

#include "ITable.h"
#include "Player.h"
#include "ILog.h"

namespace clnt
{

class ActionsParser
{
	typedef std::vector<ITable::Actions> Actions;
public:
	ActionsParser(ILog& logger, Actions& actions, pcmn::Player::List& players);

	bool Parse(bool isNeedDecision, std::string& button);

private:

	//! Parse by ante
	bool ParseByAnte(std::string& button);

	//! Parse by blinds
	bool ParseByBlinds(std::string& button, bool isNeedDecision);

	//! Link players
	void LinkPlayers(pcmn::Player::List& players);

	//! Remove unused actions
	void RemoveUselessActions();

	//! Get player by name
	pcmn::Player::Ptr GetPlayer(const std::string& name);

	//! Create unknown player
	void CreateUnknown();

	//! Insert big blind
	void InsertBigBlind();

	//! Parse one action
	bool ParseOneAction(std::string& button, bool isNeedDecision);

	//! Parse player list by actions
	void ParsePlayerListByActions(std::vector<std::string>& players, bool isNeedDecision);

	//! Match player list with old players
	bool ArePlayersMatches(const std::vector<std::string>& players);

private:

	//! Logger
	ILog& m_Log;

	//! Actions list
	Actions& m_Actions;

	//! Players list
	pcmn::Player::List& m_Players;
};

} // namespace clnt

#endif // ActionsParser_h__
