#ifndef PokerStarsTable_h__
#define PokerStarsTable_h__

#include "ITable.h"
#include "Factory.h"
#include "IMessage.h"
#include "ILog.h"
#include "Evaluator.h"
#include "Cards.h"
#include "IConnection.h"

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
public:
	Table(ILog& logger, const net::IConnection::Ptr& connection);

private:
	virtual void HandleMessage(const dasm::WindowMessage& message) override;
	virtual void PlayerAction(const std::string& name, pcmn::Action::Value, std::size_t amount) override;
	virtual void FlopCards(const pcmn::Card::List& cards) override;
	virtual void BotCards(const pcmn::Card& first, const pcmn::Card& second) override;
	virtual void PlayersInfo(const pcmn::Player::List& players) override;
	virtual void PlayerCards(const std::string& name, const pcmn::Card::List& cards) override;
private:

	//! Get player on table
	pcmn::Player& GetPlayer(const std::string& name);

	//! Get next player on table
	pcmn::Player& GetNextPlayer(const std::string& name);

	//! Get next active player on table
	pcmn::Player& GetNextActivePlayer(const std::string& name);

	//! Get previous player
	pcmn::Player& GetPreviousPlayer(const std::string& name);

	//! On bot action
	void OnBotAction();

	//! Checks for current stage completion
	bool IsPhaseCompleted(pcmn::Player& current, std::size_t& playersInPot);

	//! Reset phase
	void ResetPhase();

	//! Process winners
	void ProcessWinners(const std::size_t playersInPot);

	//! Send statistics to server
	void SendStatistic();

	//! Clear player bets
	void SetPhase(const Phase::Value phase);

	//! Close window
	void CloseWindow();


private:

	//! Message factory
	static Factory					s_Factory;

	//! Logger 
	ILog&							m_Log;

	//! Players
	pcmn::Player::List				m_Players;

	//! Pot
	std::size_t						m_Pot;

	//! Game phase
	Phase::Value					m_Phase;

	//! Flop cards
	pcmn::Card::List				m_FlopCards;

	//! Small blind size
	std::size_t						m_SmallBlindAmount;

	//! Hands evaluator
	std::auto_ptr<pcmn::Evaluator>	m_Evaluator;

	//! This game actions
	GameActions						m_Actions;

	//! Player name on button
	std::string						m_OnButton;

	//! Data sender interface
	net::IConnection::Ptr			m_Connection;

};

} // namespace ps
} // namespace clnt
#endif // PokerStarsTable_h__
