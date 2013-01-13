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
	virtual void PlayerCards(const Card& first, const Card& second) override;
	virtual void PlayersInfo(const Player::List& players) override;

private:

	//! Message factory
	static Factory s_Factory;

	//! Logger 
	ILog& m_Log;
};

} // namespace ps
} // namespace clnt
#endif // PokerStarsTable_h__
