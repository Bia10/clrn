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
    
    srv::HandsDetector::Result result;

    {
        srv::HandsDetector::Result result;
        const Card::List board = boost::assign::list_of(Card(Card::Five, Suit::Diamonds))(Card(Card::Jack, Suit::Hearts))(Card(Card::Eight, Suit::Hearts))(Card(Card::Six, Suit::Spades))(Card(Card::Six, Suit::Diamonds));

        pcmn::Player testPlayer("Eskitex2013", 0);
        testPlayer.PushAction(0, Action::Call, BetSize::Low, Player::Position::Later, Action::BigBlind, BetSize::Low);
        testPlayer.PushAction(1, Action::Bet, BetSize::High, Player::Position::Early, Action::Unknown, BetSize::VeryLow);
        //testPlayer.PushAction(1, Action::Raise, BetSize::High, Player::Position::Later, Action::Raise, BetSize::High);

        detector.DetectHand(board, testPlayer, result, 9);
    }

    {
        srv::HandsDetector::Result result;

        const std::vector<int> boardCards = boost::assign::list_of/*(39)(30)(18);*/(5)(29)(26)(17);
        Card::List board;
        for (const int c : boardCards)
            board.push_back(Card().FromEvalFormat(c));

        pcmn::Player testPlayer("CLRN", 0);
        testPlayer.PushAction(0, Action::Call, BetSize::High, Player::Position::Later, Action::Raise, BetSize::High);
        testPlayer.PushAction(1, Action::Bet, BetSize::High, Player::Position::Middle, Action::Unknown, BetSize::VeryLow);
        testPlayer.PushAction(1, Action::Raise, BetSize::High, Player::Position::Later, Action::Raise, BetSize::High);

        detector.DetectHand(board, testPlayer, result, 9);
    }

    {
        srv::HandsDetector::Result result;

        const std::vector<int> boardCards = boost::assign::list_of/*(39)(30)(18);*/(5)(29)(26)(17);
        Card::List board;
        for (const int c : boardCards)
            board.push_back(Card().FromEvalFormat(c));

        pcmn::Player testPlayer("CLRN", 0);
        testPlayer.PushAction(0, Action::Call, BetSize::High, Player::Position::Later, Action::Raise, BetSize::High);
        testPlayer.PushAction(1, Action::Bet, BetSize::High, Player::Position::Middle, Action::Unknown, BetSize::VeryLow);

        detector.DetectHand(board, testPlayer, result, 9);
    }
}

TEST(Detector, Simple)
{
    Do();
}