#ifndef EVDecisionMaker_h__
#define EVDecisionMaker_h__

#include "TableLogic.h"
#include "ILog.h"
#include "WinRate.h"
#include "Player.h"
#include "IConnection.h"
#include "IStatistics.h"

#include <boost/noncopyable.hpp>

namespace pcmn
{
	class Evaluator;
}

namespace srv
{

class EVDecisionMaker : public pcmn::ITableLogicCallback, boost::noncopyable
{
public:
	EVDecisionMaker
	(
		ILog& logger, 
		const pcmn::Evaluator& evaluator, 
		IStatistics& stats, 
		net::IConnection& connection
	);

private:

	virtual void MakeDecision(const pcmn::Player& player, 
		const pcmn::Player::Queue& activePlayers,
		const pcmn::TableContext& context,
		const pcmn::Player::Position::Value position) override;

    virtual void SendRequest(const net::Packet& packet, bool statistics) override;
    virtual void WriteStatistics(pcmn::TableContext::Data& data) override;

private:

	//! Logger
	ILog& m_Log;

	//! Evaluator
	const pcmn::Evaluator& m_Evaluator;

	//! Statistics
	srv::IStatistics& m_Stat;

	//! Client connection
	net::IConnection& m_Connection;
};

} // namespace srv
#endif // EVDecisionMaker_h__