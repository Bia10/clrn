#include "Hand.h"
#include "Cards.h"

#include <gtest/gtest.h>

#include <boost/assign.hpp>

using testing::Range;
using testing::Combine;
using testing::Values;
using namespace pcmn;

typedef std::tuple<pcmn::Card::List, pcmn::Card::List, pcmn::Hand::Value, bool> HandsTestParams;
unsigned g_CaseCounter = 0;

class HandsTest : public testing::TestWithParam<HandsTestParams>
{
public:

    void Do()
    {
        ++g_CaseCounter;
        Hand h;

        const Card::List& player = std::get<0>(GetParam());
        const Card::List& board = std::get<1>(GetParam());
        const Hand::Value hand = std::get<2>(GetParam());

        h.Parse(player, board);

        if (std::get<3>(GetParam()))
            EXPECT_TRUE(!!(h.GetValue() & hand));
        else
            EXPECT_FALSE(!!(h.GetValue() & hand));
    }
};


TEST_P(HandsTest, Parse)
{
    Do();
}

const Card::List emptyBoard;

// pocket hands
const Card::List trash = boost::assign::list_of
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Five, Suit::Spades));

const Card::List suited1 = boost::assign::list_of
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Five, Suit::Clubs));

const Card::List suited2 = boost::assign::list_of
    (Card(Card::Three, Suit::Hearts))
    (Card(Card::Nine, Suit::Hearts));

const Card::List notSuited1 = boost::assign::list_of
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Five, Suit::Spades));

const Card::List notSuited2 = boost::assign::list_of
    (Card(Card::Three, Suit::Hearts))
    (Card(Card::Nine, Suit::Spades));

const Card::List connectors1 = boost::assign::list_of
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Three, Suit::Spades));

const Card::List connectors2 = boost::assign::list_of
    (Card(Card::Queen, Suit::Hearts))
    (Card(Card::Jack, Suit::Hearts));

const Card::List connectors3 = boost::assign::list_of
    (Card(Card::Seven, Suit::Hearts))
    (Card(Card::Nine, Suit::Hearts));

const Card::List notConnectors1 = boost::assign::list_of
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Ace, Suit::Spades));

const Card::List notConnectors2 = boost::assign::list_of
    (Card(Card::Ace, Suit::Hearts))
    (Card(Card::King, Suit::Spades));

const Card::List notConnectors3 = boost::assign::list_of
    (Card(Card::Three, Suit::Hearts))
    (Card(Card::Six, Suit::Spades));

const Card::List aceKing = boost::assign::list_of
    (Card(Card::Ace, Suit::Clubs))
    (Card(Card::King, Suit::Spades));

const Card::List aceAndSmall = boost::assign::list_of
    (Card(Card::Ace, Suit::Clubs))
    (Card(Card::Three, Suit::Spades));

const Card::List aces = boost::assign::list_of
    (Card(Card::Ace, Suit::Clubs))
    (Card(Card::Ace, Suit::Spades));

const Card::List sevens = boost::assign::list_of
    (Card(Card::Seven, Suit::Clubs))
    (Card(Card::Seven, Suit::Spades));

const Card::List deuces = boost::assign::list_of
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Two, Suit::Spades));

const Card::List oneHigh1 = boost::assign::list_of
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Jack, Suit::Clubs));

const Card::List oneHigh2 = boost::assign::list_of
    (Card(Card::Queen, Suit::Hearts))
    (Card(Card::Three, Suit::Hearts));

const Card::List oneHigh3 = boost::assign::list_of
    (Card(Card::Seven, Suit::Hearts))
    (Card(Card::Ace, Suit::Hearts));

const Card::List noOneHigh1 = boost::assign::list_of
    (Card(Card::King, Suit::Clubs))
    (Card(Card::Ace, Suit::Spades));

const Card::List noOneHigh2 = boost::assign::list_of
    (Card(Card::Two, Suit::Hearts))
    (Card(Card::Three, Suit::Spades));

const Card::List noOneHigh3 = boost::assign::list_of
    (Card(Card::Five, Suit::Hearts))
    (Card(Card::Six, Suit::Spades));

const Card::List bothHigh1 = boost::assign::list_of
    (Card(Card::Ace, Suit::Clubs))
    (Card(Card::Jack, Suit::Clubs));

const Card::List bothHigh2 = boost::assign::list_of
    (Card(Card::Queen, Suit::Hearts))
    (Card(Card::King, Suit::Hearts));

const Card::List bothHigh3 = boost::assign::list_of
    (Card(Card::Queen, Suit::Hearts))
    (Card(Card::Ace, Suit::Hearts));

const Card::List noBothHigh1 = boost::assign::list_of
    (Card(Card::King, Suit::Clubs))
    (Card(Card::Two, Suit::Spades));

const Card::List noBothHigh2 = boost::assign::list_of
    (Card(Card::Two, Suit::Hearts))
    (Card(Card::Three, Suit::Spades));

const Card::List noBothHigh3 = boost::assign::list_of
    (Card(Card::Five, Suit::Hearts))
    (Card(Card::Queen, Suit::Spades));

// hands with board
const Card::List straightDraw1 = boost::assign::list_of
    (Card(Card::King, Suit::Spades))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::Four, Suit::Hearts))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Seven, Suit::Clubs));

const Card::List straightDraw2 = boost::assign::list_of
    (Card(Card::King, Suit::Spades))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::Ten, Suit::Hearts))
    (Card(Card::Five, Suit::Diamonds));

const Card::List straightDraw3 = boost::assign::list_of
    (Card(Card::Six, Suit::Spades))
    (Card(Card::Eight, Suit::Clubs))
    (Card(Card::Ace, Suit::Hearts));

const Card::List notAStraightDraw1 = boost::assign::list_of
    (Card(Card::Ace, Suit::Spades))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::Four, Suit::Hearts))
    (Card(Card::Eight, Suit::Diamonds))
    (Card(Card::Seven, Suit::Clubs));

const Card::List notAStraightDraw2 = boost::assign::list_of
    (Card(Card::King, Suit::Spades))
    (Card(Card::Ace, Suit::Clubs))
    (Card(Card::Queen, Suit::Hearts))
    (Card(Card::Five, Suit::Diamonds));

const Card::List notAStraightDraw3 = boost::assign::list_of
    (Card(Card::Six, Suit::Spades))
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Ace, Suit::Hearts));

const Card::List notAStraightDraw4 = boost::assign::list_of
    (Card(Card::Six, Suit::Spades))
    (Card(Card::Seven, Suit::Clubs))
    (Card(Card::Eight, Suit::Clubs))
    (Card(Card::Nine, Suit::Hearts));

const Card::List notAStraightDraw5 = boost::assign::list_of
    (Card(Card::Queen, Suit::Clubs))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::Nine, Suit::Hearts));

const Card::List notAStraightDraw6 = boost::assign::list_of
    (Card(Card::Four, Suit::Clubs))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Six, Suit::Clubs))
    (Card(Card::Seven, Suit::Hearts));

INSTANTIATE_TEST_CASE_P
(
    ParsePlayerHands,
    HandsTest,
    Values
    (
        // suited
        HandsTestParams(suited1, emptyBoard, Hand::Suited, true),
        HandsTestParams(suited2, emptyBoard, Hand::Suited, true),
        HandsTestParams(notSuited1, emptyBoard, Hand::Suited, false),
        HandsTestParams(notSuited2, emptyBoard, Hand::Suited, false),

        // connectors
        HandsTestParams(connectors1, emptyBoard, Hand::Connectors, true),
        HandsTestParams(connectors2, emptyBoard, Hand::Connectors, true),
        HandsTestParams(connectors3, emptyBoard, Hand::Connectors, true),
        HandsTestParams(notConnectors1, emptyBoard, Hand::Connectors, false),
        HandsTestParams(notConnectors2, emptyBoard, Hand::Connectors, false),
        HandsTestParams(notConnectors3, emptyBoard, Hand::Connectors, false),

        // one high
        HandsTestParams(oneHigh1, emptyBoard, Hand::OneHigh, true),
        HandsTestParams(oneHigh2, emptyBoard, Hand::OneHigh, true),
        HandsTestParams(oneHigh3, emptyBoard, Hand::OneHigh, true),
        HandsTestParams(noOneHigh1, emptyBoard, Hand::OneHigh, false),
        HandsTestParams(noOneHigh2, emptyBoard, Hand::OneHigh, false),
        HandsTestParams(noOneHigh3, emptyBoard, Hand::OneHigh, false),

        // both high
        HandsTestParams(bothHigh1, emptyBoard, Hand::BothHigh, true),
        HandsTestParams(bothHigh2, emptyBoard, Hand::BothHigh, true),
        HandsTestParams(bothHigh3, emptyBoard, Hand::BothHigh, true),
        HandsTestParams(noBothHigh1, emptyBoard, Hand::BothHigh, false),
        HandsTestParams(noBothHigh2, emptyBoard, Hand::BothHigh, false),
        HandsTestParams(noBothHigh3, emptyBoard, Hand::BothHigh, false),

        // ace
        HandsTestParams(aceKing, emptyBoard, Hand::Ace, true),
        HandsTestParams(aceAndSmall, emptyBoard, Hand::Ace, true),
        HandsTestParams(aces, emptyBoard, Hand::Ace, true),
        HandsTestParams(trash, emptyBoard, Hand::Ace, false),
        HandsTestParams(connectors3, emptyBoard, Hand::Ace, false),
        HandsTestParams(notSuited1, emptyBoard, Hand::Ace, false),

        // pocket pair
        HandsTestParams(aces, emptyBoard, Hand::PoketPair, true),
        HandsTestParams(sevens, emptyBoard, Hand::PoketPair, true),
        HandsTestParams(deuces, emptyBoard, Hand::PoketPair, true),
        HandsTestParams(trash, emptyBoard, Hand::PoketPair, false),
        HandsTestParams(connectors3, emptyBoard, Hand::PoketPair, false),
        HandsTestParams(notSuited1, emptyBoard, Hand::PoketPair, false),

        // straight draw
        HandsTestParams(connectors1, straightDraw1, Hand::StraightDraw, true),
        HandsTestParams(connectors2, straightDraw2, Hand::StraightDraw, true),
        HandsTestParams(connectors3, straightDraw3, Hand::StraightDraw, true),
        HandsTestParams(connectors1, straightDraw1, Hand::LowKicker, true),
        HandsTestParams(connectors3, straightDraw3, Hand::TopKicker, true),

        HandsTestParams(connectors1, notAStraightDraw1, Hand::StraightDraw, false),
        HandsTestParams(connectors2, notAStraightDraw2, Hand::StraightDraw, false),
        HandsTestParams(connectors3, notAStraightDraw3, Hand::StraightDraw, false),
        HandsTestParams(aces, notAStraightDraw4, Hand::StraightDraw, false),
        HandsTestParams(aceKing, notAStraightDraw5, Hand::StraightDraw, false),
        HandsTestParams(aceKing, notAStraightDraw6, Hand::StraightDraw, false)
    )
);

