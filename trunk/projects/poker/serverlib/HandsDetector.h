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
    //! Hand description
    struct HandDesc
    {
        typedef std::vector<HandDesc> List;
        HandDesc(const pcmn::Card::List& list = pcmn::Card::List()) 
            : m_Weight()
            , m_Cards(list)
        {}
        pcmn::Card::List m_Cards;
        float m_Weight;
    };

    //! Possible hands type
    typedef std::map<pcmn::Hand::Value, float> HandsMap;

public:

    //! Detection result type
    typedef std::map<pcmn::Hand::Value, float> Result;

    //! Ctor
    HandsDetector(ILog& log, IStatistics& stats);

    //! Detect player hand
    void DetectHand(const pcmn::Card::List& board, const pcmn::Player& player, Result& result, unsigned totalPlayers);

private:

    //! Filter cards by hands
    void CalculateWeightsByPossibleHands(const HandsMap& hands, unsigned street);

    //! Get all possible hands by cards weights
    void GetAllPossibleHands(Result& result, unsigned street);

private:
    ILog& m_Log;
    IStatistics& m_Statistic;
    HandDesc::List m_PossibleCards;
    pcmn::Card::List m_Board;
};

}

#endif // HandsDetector_h__
