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
public:

    //! Detection result type
    typedef std::map<pcmn::Hand::Value, float> Result;

    //! Ctor
    HandsDetector(ILog& log, IStatistics& stats);

    //! Detect player hand
    void DetectHand(const pcmn::Card::List& board, const pcmn::Player& player, Result& result);

private:

    float GetHandChance(pcmn::Hand::Value preflopHand, const pcmn::Card::List& board, pcmn::Hand::Value possibleHand);

private:
    ILog& m_Log;
    IStatistics& m_Statistic;
};

}

#endif // HandsDetector_h__
