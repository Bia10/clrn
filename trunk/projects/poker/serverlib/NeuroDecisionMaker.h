#ifndef DecisionMaker_h__
#define DecisionMaker_h__

#include "TableLogic.h"
#include "ILog.h"
#include "WinRate.h"
#include "Player.h"
#include "../neuro/INeuroNetwork.h"
#include "IConnection.h"
#include "IStatistics.h"
#include "Danger.h"

#include <boost/noncopyable.hpp>

namespace pcmn
{
	class Evaluator;
}

namespace srv
{

class NeuroDecisionMaker : public pcmn::ITableLogicCallback, boost::noncopyable
{
public:
	NeuroDecisionMaker
	(
		ILog& logger, 
		const pcmn::Evaluator& evaluator, 
		IStatistics& stats, 
		neuro::INetwork<float>& net,
		net::IConnection& connection
	);

private:

	virtual void MakeDecision(const pcmn::Player& player, 
		const pcmn::Player::Queue& activePlayers,
		const pcmn::TableContext& context,
		const pcmn::Player::Position::Value position) override;

private:

	//! Get player win rate
	float GetPlayerWinRate(const pcmn::Player& player, const pcmn::TableContext& context, const  pcmn::Player::Queue& activePlayers) const;

	//! Get most aggressive player style
	pcmn::Danger::Value GetDanger(const pcmn::Player& player, const  pcmn::Player::Queue& activePlayers, float botRate, unsigned street) const;

	//! Get unusual player style
	pcmn::Player::Style::Value GetBotAverageStyle(const pcmn::Player& player, const  pcmn::Player::Queue& activePlayers, unsigned totalPlayers) const;

	//! Get bot style
	pcmn::Player::Style::Value GetBotStyle(const pcmn::Player& bot, unsigned street) const;

    //! Get max stack size
    unsigned GetMaxStack(const pcmn::Player::Queue& activePlayers) const;

    //! Get max stack size
    unsigned GetMinStack(const pcmn::Player::Queue& activePlayers) const;

	//! Get player by name
	const pcmn::Player& GetPlayer(const  pcmn::Player::Queue& activePlayers, const std::string& name) const;

    virtual void SendRequest(const net::Packet& packet, bool statistics) override;

    virtual void WriteStatistics(pcmn::TableContext::Data& data) override;

private:

	//! Logger
	ILog& m_Log;

	//! Evaluator
	const pcmn::Evaluator& m_Evaluator;

	//! Statistics
	srv::IStatistics& m_Stat;

	//! Neuro network
	neuro::INetwork<float>& m_Net;

	//! Client connection
	net::IConnection& m_Connection;
};

}

#endif // DecisionMaker_h__