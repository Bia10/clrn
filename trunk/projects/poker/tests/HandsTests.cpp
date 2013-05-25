#include "Hand.h"
#include "Cards.h"

#include <gtest/gtest.h>

#include <boost/assign.hpp>

using testing::Range;
using testing::Combine;
using testing::Values;
using namespace pcmn;

typedef std::tuple<pcmn::Card::List, pcmn::Card::List, pcmn::Hand::Value, bool> HandsTestParams;

class HandsTest : public testing::TestWithParam<HandsTestParams>
{
public:

    void Do()
    {
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

const Card::List trash1 = boost::assign::list_of
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Five, Suit::Spades));

const Card::List trash2 = boost::assign::list_of
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Nine, Suit::Spades));

const Card::List trash3 = boost::assign::list_of
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Seven, Suit::Spades));

const Card::List notTrash1 = boost::assign::list_of
    (Card(Card::Ten, Suit::Clubs))
    (Card(Card::Nine, Suit::Spades));

const Card::List notTrash2 = boost::assign::list_of
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Five, Suit::Clubs));

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
    (Card(Card::Three, Suit::Clubs));

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

INSTANTIATE_TEST_CASE_P
(
    ParsePlayerHands,
    HandsTest,
    Values
    (
        // trash
        HandsTestParams(trash1, emptyBoard, Hand::Trash, true),
        HandsTestParams(trash2, emptyBoard, Hand::Trash, true),
        HandsTestParams(trash3, emptyBoard, Hand::Trash, true),
        HandsTestParams(notTrash1, emptyBoard, Hand::Trash, false),
        HandsTestParams(notTrash2, emptyBoard, Hand::Trash, false),
        HandsTestParams(aces, emptyBoard, Hand::Trash, false),

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

        // ace
        HandsTestParams(aceKing, emptyBoard, Hand::Ace, true),
        HandsTestParams(aceAndSmall, emptyBoard, Hand::Ace, true),
        HandsTestParams(aces, emptyBoard, Hand::Ace, true),
        HandsTestParams(trash1, emptyBoard, Hand::Ace, false),
        HandsTestParams(connectors3, emptyBoard, Hand::Ace, false),
        HandsTestParams(notSuited1, emptyBoard, Hand::Ace, false),

        // pocket pair
        HandsTestParams(aces, emptyBoard, Hand::PoketPair, true),
        HandsTestParams(sevens, emptyBoard, Hand::PoketPair, true),
        HandsTestParams(deuces, emptyBoard, Hand::PoketPair, true),
        HandsTestParams(trash1, emptyBoard, Hand::PoketPair, false),
        HandsTestParams(connectors3, emptyBoard, Hand::PoketPair, false),
        HandsTestParams(notSuited1, emptyBoard, Hand::PoketPair, false)
    )
);

