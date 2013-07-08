#ifndef EVDecisionMaker_h__
#define EVDecisionMaker_h__

#include "TableLogic.h"
#include "ILog.h"
#include "WinRate.h"
#include "Player.h"
#include "IConnection.h"
#include "IStatistics.h"
#include "HandsDetector.h"

#include <boost/noncopyable.hpp>

namespace pcmn
{
	class Evaluator;
}

namespace srv
{

//! Expected value based decision maker
class EVDecisionMaker : public pcmn::ITableLogicCallback, boost::noncopyable
{
    //! Player description
    struct Player
    {
        //! Player list
        typedef std::vector<Player> List;

        //! Extract chance of reaction
        float GetReaction(pcmn::BetSize::Value bet, pcmn::Action::Value action) const;

        pcmn::Player m_Player;                              //!< player data
        HandsDetector::Result m_Hands;                      //!< expected player hands
        IStatistics::PlayerInfo::BetsMap m_ReactionsOnBet;  //!< player reactions on different bets
    };

    //! Current table context description
    struct Table
    {
        unsigned m_Pot;     //!< pot size before action
        unsigned m_Bet;     //!< bet size
    };

    //! Best EV description
    struct BestEV
    {
        pcmn::Action::Value m_Action;
        float m_Value;
    };


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

    //! Calculate best EV
    BestEV CalculateBestEV(const Table& table, const Player::List& opps);

    //! Get win rate
    float GetWinRate(pcmn::Hand::Value hand);

    //! Calculate player call EV
    float CalculateCallEV(const Table& table, const Player& opp);

    //! Calculate player EV for bet
    float CalculateBetEV(const Table& table, const Player& opp, pcmn::BetSize::Value bet);

    //! Prepare the list of opponents
    Player::List PrepareOpponents();

private:

	//! Logger
	ILog& m_Log;

	//! Evaluator
	const pcmn::Evaluator& m_Evaluator;

	//! Statistics
	srv::IStatistics& m_Stat;

	//! Client connection
	net::IConnection& m_Connection;

    //! Bot
    pcmn::Player m_Bot;

    //! Flop cards
    pcmn::Card::List m_Flop;

    //! Big blind amount
    unsigned m_BigBlind;

    //! Opponents
    pcmn::Player::List m_Opponents;

    //! Max bet before player
    unsigned m_MaxBet;
};

} // namespace srv
#endif // EVDecisionMaker_h__