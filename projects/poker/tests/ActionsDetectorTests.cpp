#include "Board.h"
#include "Conversion.h"
#include "../serverlib/MongoStatistics.h"
#include "../serverlib/HandsDetector.h"
#include "../serverlib/ActionsDetector.h"
#include "Log.h"
#include "Modules.h"

#include <gtest/gtest.h>

#include <boost/assign.hpp>

using testing::Range;
using testing::Combine;
using testing::Values;
using namespace pcmn;

void DoActionsDetect()
{
    Log log;
    log.Open("1", Modules::DataBase, ILog::Level::Trace);
    
    srv::MongoStatistics stats(log);
    srv::ActionsDetector detector(log, stats);

    srv::HandsDetector::Result hands;
    srv::ActionsDetector::OutputActions actions;
    detector.DetectAtions("CLRN", pcmn::Player::Position::Middle, hands, 1, 8, pcmn::Board::Unknown, actions);
}

TEST(ActionsDetector, Simple)
{
    DoActionsDetect();
}