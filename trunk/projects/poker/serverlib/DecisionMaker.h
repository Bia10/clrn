#ifndef DecisionMaker_h__
#define DecisionMaker_h__

#include "Logic.h"
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

class DecisionMaker : public pcmn::IDecisionCallback, boost::noncopyable
{
public:
	DecisionMaker
	(
		ILog& logger, 
		const pcmn::Evaluator& evaluator, 
		const IStatistics& stats, 
		neuro::INetwork<float>& net,
		net::IConnection& connection
	);

private:

	virtual void MakeDecision(const pcmn::Player& player, 
		const PlayerQueue& activePlayers,
		const pcmn::TableContext& context,
		const pcmn::Player::Position::Value position) override;

private:

	//! Get player win rate
	float GetPlayerWinRate(const pcmn::Player& player, const pcmn::TableContext& context, const PlayerQueue& activePlayers) const;

	//! Get most aggressive player style
	pcmn::Danger::Value GetDanger(const pcmn::Player& player, const PlayerQueue& activePlayers, float botRate) const;

	//! Get unusual player style
	pcmn::Player::Style::Value GetBotAverageStyle(const pcmn::Player& player, const PlayerQueue& activePlayers) const;

	//! Get bot style
	pcmn::Player::Style::Value GetBotStyle(const pcmn::Player& bot) const;

	//! Get max stack size
	unsigned GetMaxStack(const PlayerQueue& activePlayers) const;

	//! Get player by name
	const pcmn::Player& GetPlayer(const PlayerQueue& activePlayers, const std::string& name) const;

private:

	//! Logger
	ILog& m_Log;

	//! Evaluator
	const pcmn::Evaluator& m_Evaluator;

	//! Statistics
	const srv::IStatistics& m_Stat;

	//! Neuro network
	neuro::INetwork<float>& m_Net;

	//! Client connection
	net::IConnection& m_Connection;
};

}

#endif // DecisionMaker_h__