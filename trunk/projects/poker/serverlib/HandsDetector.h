#ifndef HandsDetector_h__
#define HandsDetector_h__

#include "ILog.h"
#include "IStatistics.h"
#include "Hand.h"
#include "Player.h"
#include "Cards.h"

#include <map>

namespace srv
{

//! Hands recognizer
class HandsDetector
{
    //! List of hands
    typedef std::vector<pcmn::Card::List> Hands;

    //! Hands cache
    typedef std::map<pcmn::Hand::Value, Hands> Cache;

public:

    //! Detection result type
    typedef std::map<pcmn::Hand::Value, float> Result;

    //! Ctor
    HandsDetector(ILog& log, IStatistics& stats);

    //! Detect player hand
    void DetectHand(const pcmn::Card::List& board, const pcmn::Player& player, Result& result, unsigned totalPlayers);

private:

    //! Get chance of the specified by characteristics preflop hand became a specified flop hand
    float GetFlopHandChanceByPreflopHand(pcmn::Hand::Value preflopHand, const pcmn::Card::List& board, pcmn::Hand::Value possibleHand);

    //! Get chance of the specified preflop hand with determined flop hand
    float GetPreflopHandChanceByFlopHand(pcmn::Hand::Value preflopHand, const pcmn::Card::List& board, pcmn::Hand::Value flopHand);

    //! Filter cards by hands
    void FilterCardsByPossibleHands(const std::vector<pcmn::Hand::Value>& hands, unsigned street);

    //! Check if hand is possible
    bool IsHandPossible(pcmn::Hand::Value hand, pcmn::Board::Value board) const;

    //! Get hand chance by cards and board
    float GetHandChance(pcmn::Hand::Value hand) const;

private:
    ILog& m_Log;
    IStatistics& m_Statistic;
    Cache m_Cache;
    Hands m_PossibleCards;
    pcmn::Card::List m_Board;
};

}

#endif // HandsDetector_h__
