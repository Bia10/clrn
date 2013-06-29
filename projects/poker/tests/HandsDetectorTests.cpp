#include "Board.h"
#include "Conversion.h"
#include "../serverlib/MongoStatistics.h"
#include "../serverlib/HandsDetector.h"
#include "Log.h"
#include "Actions.h"
#include "Modules.h"

#include <gtest/gtest.h>

#include <boost/assign.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>

using testing::Range;
using testing::Combine;
using testing::Values;
using namespace pcmn;

class TestReadOnlyStatistics : public srv::MongoStatistics
{
public:
    TestReadOnlyStatistics(ILog& logger) : srv::MongoStatistics(logger)  {}
    virtual void Write(pcmn::TableContext::Data& data) override {}
};

void Do()
{
    Log log;
    log.Open("1", Modules::DataBase, ILog::Level::Trace);
    log.Open("1", Modules::Server, ILog::Level::Trace);
   
    TestReadOnlyStatistics stats(log);
    srv::HandsDetector detector(log, stats);
    
    const std::vector<int> boardCards = boost::assign::list_of(39)(30)(18);//(5)(29)(26)(17);
    Card::List board;
    for (const int c : boardCards)
        board.push_back(Card().FromEvalFormat(c));

    srv::HandsDetector::Result result1;
    srv::HandsDetector::Result result2;

/*
    {
        pcmn::Player testPlayer("CLRN", 0);
        testPlayer.PushAction(0, Action::Call, BetSize::Low, Player::Position::Middle, Action::Call, BetSize::Low);
        testPlayer.PushAction(1, Action::Bet, BetSize::High, Player::Position::Middle, Action::Call, BetSize::Low);
        testPlayer.PushAction(1, Action::Raise, BetSize::High, Player::Position::Later, Action::Call, BetSize::Low);

        Card::List board1 = board;
        board1.resize(3);
        detector.DetectHand(board1, testPlayer, result1, 9);
    }*/

    {
        pcmn::Player testPlayer("CLRN", 0);
        testPlayer.PushAction(0, Action::Call, BetSize::High, Player::Position::Later, Action::Raise, BetSize::High);
        testPlayer.PushAction(1, Action::Bet, BetSize::High, Player::Position::Middle, Action::Unknown, BetSize::VeryLow);

        Card::List board2 = board;
        board2.resize(3);
        detector.DetectHand(board2, testPlayer, result2, 9);
    }
}

TEST(Detector, Simple)
{
    Do();
}