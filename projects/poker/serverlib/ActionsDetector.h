#ifndef ActionsDetector_h__
#define ActionsDetector_h__

#include "ILog.h"
#include "Hand.h"
#include "BetSize.h"
#include "Player.h"
#include "IStatistics.h"
#include "Board.h"

#include <map>
#include <string>

namespace srv
{

class ActionsDetector
{
public:

    //! Input data type
    typedef std::map<pcmn::Hand::Value, float> InputHands;

    //! Output data type
    typedef std::map<pcmn::BetSize::Value, float> OutputActions;

    ActionsDetector(ILog& logger, const IStatistics& stats);

    void DetectAtions
    (
        const std::string& name, 
        pcmn::Player::Position::Value pos, 
        const InputHands& hands, 
        unsigned street,
        unsigned playersCount,
        pcmn::Board::Value board,
        OutputActions& result
    );

private:

    ILog& m_Log;
    const IStatistics& m_Stats;
};

} // namespace srv

#endif // ActionsDetector_h__
