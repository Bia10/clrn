#ifndef PokerStarsTable_h__
#define PokerStarsTable_h__

#include "ITable.h"
#include "Factory.h"
#include "IMessage.h"
#include "ILog.h"
#include "Evaluator.h"
#include "Cards.h"
#include "IConnection.h"
#include "ActionsParser.h"
#include "TableLogic.h"

#include <map>

namespace clnt
{
namespace ps
{

class Table : public ITable, public pcmn::ITableLogicCallback
{
	typedef cmn::functional::Factory<IMessage, std::size_t, cmn::functional::IgnoreErrorPolicy> Factory;
	typedef std::vector<Actions> GameActions;
	typedef std::map<std::string, pcmn::Card::List> Cards;
	typedef std::map<std::string, bool> ActiveMap;
public:
	Table(ILog& logger, HWND window, const net::IConnection::Ptr& connection);

private:
	virtual void HandleMessage(const dasm::WindowMessage& message) override;
	virtual void PlayerAction(const std::string& name, pcmn::Action::Value, std::size_t amount) override;
	virtual void FlopCards(const pcmn::Card::List& cards) override;
	virtual void BotCards(const pcmn::Card& first, const pcmn::Card& second) override;
	virtual void PlayersInfo(const pcmn::Player::List& players) override;
	virtual void PlayerCards(const std::string& name, const pcmn::Card::List& cards) override;
    virtual void SendRequest(const net::Packet& packet) override;
    virtual void MakeDecision(const pcmn::Player& player, const pcmn::Player::Queue& activePlayers, const pcmn::TableContext& context, const pcmn::Player::Position::Value position) override;

private:

	//! Get player on table
	pcmn::Player::Ptr GetPlayer(const std::string& name);

	//! On bot action
	void OnBotAction();

	//! Reset phase
	void ResetPhase();

	//! Send statistics to server
	void SendStatistic();

	//! Clear player bets
	void SetPhase(const Phase::Value phase);

	//! Close window
	void CloseTableWindow();

	//! Server reply callback
	void ReceiveFromServerCallback(const google::protobuf::Message& message);

	//! Press button
	void PressButton(const float x, const float y);

	//! Press button thread
	void PressButtonThread(const float x, const float y);

	//! Fold
	void Fold();

	//! Call or check
	void CheckCall();

	//! Bet or raise
	void BetRaise(unsigned amount);

	//! Erase player
	void ErasePlayer(const std::string& name);

	//! Make a decision if next
	void MakeDecisionIfNext(const std::string& current);

private:

	//! Message factory
	static Factory					s_Factory;

	//! Logger 
	ILog&							m_Log;

	//! Window handle
	const HWND						m_Window;

	//! Players
	pcmn::Player::List				m_Players;

	//! Player stacks
	StackMap						m_Stacks;

	//! Player last stacks
	StackMap						m_LastStacks;

	//! Game phase
	Phase::Value					m_Phase;

	//! Flop cards
	pcmn::Card::List				m_FlopCards;

	//! Hands evaluator
	std::auto_ptr<pcmn::Evaluator>	m_Evaluator;

	//! This game actions
	GameActions						m_Actions;

	//! Data sender interface
	net::IConnection::Ptr			m_Connection;

	//! Player cards
	Cards							m_PlayerCards;	

	//! Is need decision
	bool							m_IsNeedDecision;

	//! Actions parser
	ActionsParser					m_ActionsParser;

	//! Player bets
	StackMap						m_Bets;

	//! Player total bets
	StackMap						m_TotalBets;

	//! Losers
	std::vector<std::string>		m_Loosers;

	//! Folded players
	ActiveMap						m_FoldedPlayers;

	//! Players which waiting for game completion
	ActiveMap						m_WaitingPlayers;

	//! Player on button
	std::string						m_Button;

	//! Is cards showed / game finished
	bool							m_IsCardsShowed;

    //! Table logic 
    pcmn::TableLogic                m_Logic;
};

} // namespace ps
} // namespace clnt
#endif // PokerStarsTable_h__
