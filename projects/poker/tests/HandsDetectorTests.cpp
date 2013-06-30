#include "Board.h"
#include "Conversion.h"
#include "../serverlib/MongoStatistics.h"
#include "../serverlib/HandsDetector.h"
#include "Log.h"
#include "Actions.h"
#include "Modules.h"
#include "Utils.h"
#include "ITable.h"

#include <gtest/gtest.h>

#include <boost/assign.hpp>
#include <boost/filesystem.hpp>

using testing::Range;
using testing::Combine;
using testing::Values;
using namespace pcmn;

typedef srv::IStatistics::PlayerInfo::Hands Hands;
typedef std::vector<Hands> DBHands;

class MockedStats : public srv::IStatistics
{
public:
    MockedStats(const DBHands& hands) : m_Hands(hands)
    {
    }

    virtual void Write(pcmn::TableContext::Data& data) 
    {
        throw std::exception("The method or operation is not implemented.");
    }

    virtual unsigned GetRanges(PlayerInfo::List& players) const
    {
        throw std::exception("The method or operation is not implemented.");
    }

    virtual pcmn::Player::Style::Value GetAverageStyle(const std::string& target, const std::string& opponent) const
    {
        throw std::exception("The method or operation is not implemented.");
    }

    virtual void GetEquities(PlayerInfo::List& players, unsigned street) const
    {
        throw std::exception("The method or operation is not implemented.");
    }

    virtual void GetHands(PlayerInfo& player, unsigned street, unsigned count) const
    {
        player.m_Hands = m_Hands[street];
    }

private:
    DBHands m_Hands;
};

class ReadOnlyStatistics : public srv::MongoStatistics
{
public:
    ReadOnlyStatistics(ILog& logger) : srv::MongoStatistics(logger)
    {

    }


    virtual void Write(pcmn::TableContext::Data& data) override
    {

    }
};

// db stats, board, cards
typedef std::tuple<DBHands, pcmn::Card::List, pcmn::Card::List> DetectorParams;

class DetectorTest : public testing::TestWithParam<DetectorParams>
{
public:
    void Do()
    {
        const DBHands& dbHands = std::get<0>(GetParam());
        const Card::List& board = std::get<1>(GetParam());
        const pcmn::Card::List& hand = std::get<2>(GetParam());

        pcmn::Player testPlayer("test", 0);

        for (unsigned i = 0; i < dbHands.size(); ++i)
            testPlayer.PushAction(i, Action::Fold, BetSize::VeryLow, Player::Position::Early, Action::Fold, BetSize::VeryLow); // fake
        const unsigned street = dbHands.size() - 1;

        MockedStats stats(dbHands);
        srv::HandsDetector detector(m_Log, stats);

        srv::HandsDetector::Result result;
        detector.DetectHand(board, testPlayer, result, 9);

        pcmn::Hand parser;
        parser.Parse(hand, board);

        pcmn::Hand::Value resultHand = parser.GetValue();
        resultHand = static_cast<pcmn::Hand::Value>(resultHand & (pcmn::Hand::FLOP_HAND_MASK | pcmn::Hand::POWER_MASK));
        if (street == 3) // remove draws from river
            resultHand = static_cast<pcmn::Hand::Value>(resultHand & ~pcmn::Hand::DRAWS_MASK);

        EXPECT_TRUE(result.count(resultHand) != 0) << "Expected '" << pcmn::Hand::ToString(resultHand) << "' in '" << LogAnyHolder(result).operator std::string() << "'";
    }

private:
    Log m_Log;
};

class DBDataHolder
{
public:
    DBDataHolder(unsigned street, unsigned hand, float percent) 
        : m_List() 
    {
        (*this)(street, hand, percent);
    }

    DBDataHolder& operator () (unsigned street, unsigned hand, float percent)
    {
        m_List.resize(street + 1);
        m_List[street][static_cast<Hand::Value>(hand)] = percent;
        return *this;
    }

    operator const DBHands& () const
    {
        return m_List;
    }

private:
    DBHands m_List;
};


/*

TEST_P(DetectorTest, Detect)
{
    Do();
}
*/

INSTANTIATE_TEST_CASE_P
(
    FromDB,
    DetectorTest,
    Values
    (
        DetectorParams
        (
            DBDataHolder
                (0, Hand::OneHigh, 0.1f)
                (0, Hand::Connectors, 0.1f)
                (0, Hand::Suited | Hand::BothHigh | Hand::Connectors, 0.1f)
                (0, Hand::Suited | Hand::OneHigh | Hand::OneLow, 0.1f)
                (0, Hand::Suited | Hand::Ace | Hand::Connectors | Hand::BothHigh, 0.1f)
                (0, Hand::Suited | Hand::Connectors | Hand::BothHigh, 0.1f)
                (0, Hand::Suited | Hand::OneLow, 0.1f)
                (0, Hand::Suited | Hand::OneLow | Hand::Connectors, 0.1f)
                (0, Hand::Suited | Hand::BothHigh, 0.1f)
                (0, Hand::Suited | Hand::Connectors, 0.05f)
                (0, Hand::Suited | Hand::OneHigh, 0.05f)
                (1, Hand::HighCard, 30.0f)
                (1, Hand::Pair | Hand::Middle, 0.3f)
                (1, Hand::Pair | Hand::Top, 0.3f)
                (1, Hand::FlushDraw, 0.1f),
            CardListHolder
                (Card(Card::Five, Suit::Clubs))
                (Card(Card::Jack, Suit::Hearts))
                (Card(Card::Eight, Suit::Clubs)),
            CardListHolder(Card(Card::Ten, Suit::Clubs))(Card(Card::Jack, Suit::Clubs))
        )
    )
);


void Do()
{
    Log log;
    log.Open("1", Modules::DataBase, ILog::Level::Trace);
    log.Open("1", Modules::Server, ILog::Level::Trace);
   
    ReadOnlyStatistics stats(log);
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

        const std::vector<int> boardCards = boost::assign::list_of(5)(29)(26)(17);
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

        const std::vector<int> boardCards = boost::assign::list_of(5)(29)(26)(17);
        Card::List board;
        for (const int c : boardCards)
            board.push_back(Card().FromEvalFormat(c));

        pcmn::Player testPlayer("CLRN", 0);
        testPlayer.PushAction(0, Action::Raise, BetSize::High, Player::Position::Later, Action::BigBlind, BetSize::Low);
        testPlayer.PushAction(1, Action::Bet, BetSize::High, Player::Position::Middle, Action::Unknown, BetSize::VeryLow);
        testPlayer.PushAction(2, Action::Bet, BetSize::High, Player::Position::Later, Action::Unknown, BetSize::VeryLow);

        detector.DetectHand(board, testPlayer, result, 9);
    }

    {
        srv::HandsDetector::Result result;

        const std::vector<int> boardCards = boost::assign::list_of(21)(28)(36)(13)(8);
        Card::List board;
        for (const int c : boardCards)
            board.push_back(Card().FromEvalFormat(c));

        const std::vector<int> playerCards = boost::assign::list_of(9)(5);
        Card::List player;
        for (const int c : playerCards)
            player.push_back(Card().FromEvalFormat(c));

        pcmn::Player testPlayer("CLRN", 0);
        testPlayer.PushAction(0, Action::Call, BetSize::Low, Player::Position::Middle, Action::BigBlind, BetSize::Low);
        testPlayer.PushAction(1, Action::Check, BetSize::VeryLow, Player::Position::Early, Action::Unknown, BetSize::VeryLow);
        testPlayer.PushAction(2, Action::Check, BetSize::VeryLow, Player::Position::Early, Action::Unknown, BetSize::VeryLow);
        testPlayer.PushAction(3, Action::Bet, BetSize::High, Player::Position::Middle, Action::Unknown, BetSize::VeryLow);

        detector.DetectHand(board, testPlayer, result, 9);
    }
    {
        srv::HandsDetector::Result result;

        const std::vector<int> boardCards = boost::assign::list_of(5)(29)(26)(17);
        Card::List board;
        for (const int c : boardCards)
            board.push_back(Card().FromEvalFormat(c));

        pcmn::Player testPlayer("CLRN", 0);
        testPlayer.PushAction(0, Action::Call, BetSize::High, Player::Position::Later, Action::Raise, BetSize::High);
        testPlayer.PushAction(1, Action::Bet, BetSize::High, Player::Position::Middle, Action::Unknown, BetSize::VeryLow);

        detector.DetectHand(board, testPlayer, result, 9);
    }
}


class TestTable : public clnt::ITable
{
public:
    TestTable()
    {
        m_Log.Open("1", Modules::DataBase, ILog::Level::Trace);
        m_Log.Open("1", Modules::Server, ILog::Level::Trace);
    }
private:

    virtual void HandleMessage(const dasm::WindowMessage& message) 
    {
    }

    virtual void PlayerAction(const std::string& name, pcmn::Action::Value action, std::size_t amount) 
    {
        if (action != Action::ShowCards)
            return;
   
        ReadOnlyStatistics stats(m_Log);
        srv::HandsDetector detector(m_Log, stats);
    
        srv::HandsDetector::Result result;
    }

    virtual void FlopCards(const pcmn::Card::List& cards) 
    {
        m_Flop = cards;
    }

    virtual void BotCards(const pcmn::Card& first, const pcmn::Card& second) 
    {
        m_Flop.clear();
    }

    virtual void PlayerCards(const std::string& name, const pcmn::Card::List& cards) 
    {
        m_Name = name;
        m_PlayerCards = cards;
    }

    virtual void PlayersInfo(const pcmn::Player::List& players) 
    {
    }
private:

    Card::List m_Flop;
    Card::List m_PlayerCards;
    std::string m_Name;
    Log m_Log;
};

void RunDetectorTest()
{
    boost::filesystem::directory_iterator it("../../../poker/logs/");
    boost::filesystem::directory_iterator end;

    for (; it != end; ++it)
    {
        if (boost::filesystem::is_directory(it->path()))
            continue;

        const std::string full = boost::filesystem::system_complete(it->path()).string();

        std::cout << "Processing: '" << full << "' ..." << std::endl;

        // read file data
        std::ifstream ifs(full.c_str(), std::ios_base::binary);
        ASSERT_TRUE(ifs.is_open()) << full;

        ifs >> std::noskipws;

        std::string content;
        std::copy(std::istream_iterator<char>(ifs), std::istream_iterator<char>(), std::back_inserter(content));

        TestTable table;
        tests::ParseData(content, table);
    }
}

TEST(Detector, ByLogs)
{
    Do();
}
