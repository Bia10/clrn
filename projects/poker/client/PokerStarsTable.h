#ifndef PokerStarsTable_h__
#define PokerStarsTable_h__

#include "ITable.h"
#include "Factory.h"
#include "IMessage.h"
#include "ILog.h"
#include "CombinationsCalculator.h"
#include "Cards.h"

namespace clnt
{
namespace ps
{

class Table : public ITable
{
	struct ActionDesc
	{
		ActionDesc(const std::string& name, Action::Value value, std::size_t amount)
			: m_Name(name)
			, m_Value(value)
			, m_Amount(amount)
		{}
		std::string m_Name;
		Action::Value m_Value;
		std::size_t m_Amount;
	};
	typedef cmn::functional::Factory<IMessage, std::size_t, cmn::functional::IgnoreErrorPolicy> Factory;
	typedef std::vector<ActionDesc> Actions;
	typedef std::vector<Actions> GameActions;
public:
	Table(ILog& logger);

private:
	virtual void HandleMessage(const dasm::WindowMessage& message) override;
	virtual void PlayerAction(const std::string& name, Action::Value, std::size_t amount) override;
	virtual void FlopCards(const Card::List& cards) override;
	virtual void BotCards(const Card& first, const Card& second) override;
	virtual void PlayersInfo(const Player::List& players) override;
	virtual void PlayerCards(const std::string& name, const Card::List& cards) override;
private:

	//! Get player on table
	Player& GetPlayer(const std::string& name);

	//! Get next player on table
	Player& GetNextPlayer(const std::string& name);

	//! Get next active player on table
	Player& GetNextActivePlayer(const std::string& name);

	//! On bot action
	void OnBotAction();

	//! Checks for current stage completion
	bool IsPhaseCompleted(Player& current, std::size_t& playersInPot);

	//! Reset phase
	void ResetPhase();

	//! Process winners
	void ProcessWinners(const std::size_t playersInPot);

	//! Send statistics to server
	void SendStatistic();

	//! Clear player bets
	void SetPhase(const Phase::Value phase);


private:

	//! Message factory
	static Factory				s_Factory;

	//! Logger 
	ILog&						m_Log;

	//! Players
	Player::List				m_Players;

	//! Pot
	std::size_t					m_Pot;

	//! Game phase
	Phase::Value				m_Phase;

	//! Flop cards
	Card::List					m_FlopCards;

	//! Flop cards
	Card::List					m_BotCards;

	//! Small blind size
	std::size_t					m_SmallBlindAmount;

	//! Hands evaluator
	std::auto_ptr<Evaluator>	m_Evaluator;

	//! This game actions
	GameActions					m_Actions;

	//! Player name on button
	std::string					m_OnButton;

};

} // namespace ps
} // namespace clnt
#endif // PokerStarsTable_h__
