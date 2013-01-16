#ifndef PokerStarsTable_h__
#define PokerStarsTable_h__

#include "ITable.h"
#include "Factory.h"
#include "IMessage.h"
#include "ILog.h"

namespace clnt
{
namespace ps
{

class Table : public ITable
{
	typedef cmn::functional::Factory<IMessage, std::size_t, cmn::functional::IgnoreErrorPolicy> Factory;
public:
	Table(ILog& logger);

private:
	virtual void HandleMessage(const dasm::WindowMessage& message) override;
	virtual void PlayerAction(const std::string& name, Action::Value, std::size_t amount) override;
	virtual void FlopCards(const std::vector<Card>& cards) override;
	virtual void BotCards(const Card& first, const Card& second) override;
	virtual void PlayersInfo(const Player::List& players) override;
	virtual void PlayerCards(const std::string& name, const std::string& cards) override;
private:

	//! Get player on table
	Player& GetPlayer(const std::string& name);

	//! Get next player on table
	Player& GetNextPlayer(const std::string& name);

	//! On bot action
	void OnBotAction();

	//! Checks for current stage completion
	bool IsPhaseCompleted(Player& current, Player& next);

	//! Reset phase
	void ResetPhase();


private:

	//! Message factory
	static Factory			s_Factory;

	//! Logger 
	ILog&					m_Log;

	//! Players
	Player::List			m_Players;

	//! Pots
	Pot::List				m_Pots;

	//! Game phase
	Phase::Value			m_Phase;

	//! Flop cards
	Card::List				m_FlopCards;

	//! Flop cards
	Card::List				m_PlayerCards;

	//! Small blind size
	std::size_t				m_SmallBlindAmount;
};

} // namespace ps
} // namespace clnt
#endif // PokerStarsTable_h__
