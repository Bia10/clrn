#ifndef PokerStarsTable_h__
#define PokerStarsTable_h__

#include "ITable.h"
#include "Factory.h"
#include "IMessage.h"
#include "ILog.h"
#include "Evaluator.h"
#include "Cards.h"
#include "IConnection.h"
#include "TableLogic.h"

#include <map>

namespace clnt
{
namespace ps
{

class Table : public ITable, public pcmn::ITableLogicCallback
{
	typedef cmn::functional::Factory<IMessage, std::size_t, cmn::functional::IgnoreErrorPolicy> Factory;
public:
	Table(ILog& logger, HWND window, const net::IConnection::Ptr& connection, const pcmn::Evaluator& evaluator);

private:
	virtual void HandleMessage(const dasm::WindowMessage& message) override;
	virtual void PlayerAction(const std::string& name, pcmn::Action::Value, std::size_t amount) override;
	virtual void FlopCards(const pcmn::Card::List& cards) override;
	virtual void BotCards(const pcmn::Card& first, const pcmn::Card& second) override;
	virtual void PlayersInfo(const pcmn::Player::List& players) override;
	virtual void PlayerCards(const std::string& name, const pcmn::Card::List& cards) override;
   
    virtual void SendRequest(const net::Packet& packet, bool statistics) override;
    virtual void MakeDecision(const pcmn::Player& player, const pcmn::Player::Queue& activePlayers, const pcmn::TableContext& context, const pcmn::Player::Position::Value position) override;
    virtual void WriteStatistics(pcmn::TableContext::Data& data) override;

private:

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

private:

	//! Message factory
	static Factory					s_Factory;

	//! Logger 
	ILog&							m_Log;

	//! Window handle
	const HWND						m_Window;

	//! Hands evaluator
	const pcmn::Evaluator&      	m_Evaluator;

	//! Data sender interface
	net::IConnection::Ptr			m_Connection;

    //! Table logic 
    pcmn::TableLogic                m_Logic;
};

} // namespace ps
} // namespace clnt
#endif // PokerStarsTable_h__
