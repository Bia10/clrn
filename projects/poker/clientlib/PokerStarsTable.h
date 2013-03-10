#ifndef PokerStarsTable_h__
#define PokerStarsTable_h__

#include "ITable.h"
#include "Factory.h"
#include "IMessage.h"
#include "ILog.h"
#include "Evaluator.h"
#include "Cards.h"
#include "IConnection.h"

#include <map>

namespace clnt
{
namespace ps
{

class Table : public ITable
{
	struct ActionDesc
	{
		ActionDesc(const std::string& name, pcmn::Action::Value value, std::size_t amount)
			: m_Name(name)
			, m_Value(value)
			, m_Amount(amount)
		{}
		std::string m_Name;
		pcmn::Action::Value m_Value;
		std::size_t m_Amount;
	};
	typedef cmn::functional::Factory<IMessage, std::size_t, cmn::functional::IgnoreErrorPolicy> Factory;
	typedef std::vector<ActionDesc> Actions;
	typedef std::vector<Actions> GameActions;
	typedef std::map<std::string, unsigned> StackMap;
	typedef std::map<std::string, pcmn::Card::List> Cards;
public:
	Table(ILog& logger, HWND window, const net::IConnection::Ptr& connection);

private:
	virtual void HandleMessage(const dasm::WindowMessage& message) override;
	virtual void PlayerAction(const std::string& name, pcmn::Action::Value, std::size_t amount) override;
	virtual void FlopCards(const pcmn::Card::List& cards) override;
	virtual void BotCards(const pcmn::Card& first, const pcmn::Card& second) override;
	virtual void PlayersInfo(const pcmn::Player::List& players) override;
	virtual void PlayerCards(const std::string& name, const pcmn::Card::List& cards) override;
	virtual void Ante(std::size_t value) override;
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

	//! Bet ante
	void BetAnte();

	//! Remove player
	void RemovePlayer(const std::string& name);

	//! Parse players
	void ParsePlayers(std::string& button);

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

	//! Table ante
	unsigned						m_Ante;

	//! Player cards
	Cards							m_PlayerCards;	

};

} // namespace ps
} // namespace clnt
#endif // PokerStarsTable_h__
