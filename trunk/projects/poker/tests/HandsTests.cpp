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
        const bool positive = std::get<3>(GetParam());

        h.Parse(player, board);

        if (positive)
            EXPECT_TRUE(!!(h.GetValue() & hand)) << "Expected that: '" << h.GetValue() << "' contains '" << hand << "'";
        else
            EXPECT_FALSE(!!(h.GetValue() & hand)) << "Expected that: '" << h.GetValue() << "' not contains '" << hand << "'";
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

const Card::List diamonds = boost::assign::list_of
    (Card(Card::Two, Suit::Diamonds))
    (Card(Card::Five, Suit::Diamonds));

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

const Card::List notSuited3 = boost::assign::list_of
    (Card(Card::Three, Suit::Hearts))
    (Card(Card::Ten, Suit::Spades));

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

const Card::List aceSeven = boost::assign::list_of
    (Card(Card::Ace, Suit::Hearts))
    (Card(Card::Seven, Suit::Spades));

// hands with board
// straight draw
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

// not a straight draw
const Card::List notAStraightDraw1 = boost::assign::list_of
    (Card(Card::Ace, Suit::Spades))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::Four, Suit::Hearts))
    (Card(Card::Eight, Suit::Diamonds))
    (Card(Card::Seven, Suit::Clubs));

const Card::List notAStraightDraw2 = boost::assign::list_of
    (Card(Card::King, Suit::Spades))
    (Card(Card::Ace, Suit::Clubs))
    (Card(Card::Queen, Suit::Diamonds))
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

// god shot
const Card::List godShot1 = boost::assign::list_of // with 2 5
    (Card(Card::Six, Suit::Spades))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Ace, Suit::Hearts))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List godShot2 = boost::assign::list_of // with 3 9
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Six, Suit::Hearts))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Seven, Suit::Clubs));

const Card::List godShot3 = boost::assign::list_of // with aces
    (Card(Card::King, Suit::Spades))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::Ten, Suit::Hearts))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Seven, Suit::Clubs));

const Card::List godShot4 = boost::assign::list_of // with aces
    (Card(Card::King, Suit::Spades))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::Queen, Suit::Hearts))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Six, Suit::Clubs));

const Card::List godShot5 = boost::assign::list_of // with aces
    (Card(Card::Two, Suit::Spades))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Four, Suit::Hearts));

// not a god shot
const Card::List notAGodShot1 = boost::assign::list_of // with 2 5
    (Card(Card::Four, Suit::Spades))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Jack, Suit::Hearts))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List notAGodShot2 = boost::assign::list_of // with 3 9
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Jack, Suit::Hearts))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Seven, Suit::Clubs));

const Card::List notAGodShot3 = boost::assign::list_of // with aces
    (Card(Card::King, Suit::Spades))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::Ten, Suit::Hearts))
    (Card(Card::Queen, Suit::Diamonds))
    (Card(Card::Seven, Suit::Clubs));

const Card::List notAGodShot4 = boost::assign::list_of // with aces
    (Card(Card::Eight, Suit::Clubs))
    (Card(Card::Seven, Suit::Hearts))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Six, Suit::Clubs));

const Card::List notAGodShot5 = boost::assign::list_of // with aces
    (Card(Card::Two, Suit::Spades))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Six, Suit::Hearts));

// flush draw
const Card::List flushDraw1 = boost::assign::list_of // with clubs
    (Card(Card::Four, Suit::Spades))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Jack, Suit::Hearts))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List flushDraw2 = boost::assign::list_of // with clubs
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Ace, Suit::Clubs))
    (Card(Card::Seven, Suit::Clubs));

const Card::List flushDraw3 = boost::assign::list_of // with hearts
    (Card(Card::King, Suit::Hearts))
    (Card(Card::Jack, Suit::Hearts))
    (Card(Card::Ten, Suit::Diamonds))
    (Card(Card::Seven, Suit::Clubs));

const Card::List flushDraw4 = boost::assign::list_of // with one heart
    (Card(Card::Eight, Suit::Hearts))
    (Card(Card::Seven, Suit::Hearts))
    (Card(Card::Five, Suit::Hearts));

const Card::List flushDraw5 = boost::assign::list_of // with one clubs
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Six, Suit::Clubs));

// not a flush draw
const Card::List notAFlushDraw1 = boost::assign::list_of // with clubs
    (Card(Card::Four, Suit::Spades))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List notAFlushDraw2 = boost::assign::list_of // with clubs
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Ace, Suit::Clubs))
    (Card(Card::Seven, Suit::Hearts));

const Card::List notAFlushDraw3 = boost::assign::list_of // with diamonds
    (Card(Card::King, Suit::Hearts))
    (Card(Card::Jack, Suit::Hearts))
    (Card(Card::Ten, Suit::Diamonds))
    (Card(Card::Seven, Suit::Clubs));

// high card
const Card::List highCard1 = boost::assign::list_of // with one ace
    (Card(Card::Four, Suit::Spades))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List highCard2 = boost::assign::list_of // with king
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Seven, Suit::Hearts));

// not a high card
const Card::List notAHighCard1 = boost::assign::list_of // with one ace
    (Card(Card::Four, Suit::Spades))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Ace, Suit::Clubs))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List notAHighCard2 = boost::assign::list_of // with 2 5
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Five, Suit::Spades))
    (Card(Card::Seven, Suit::Hearts));

// low pair
const Card::List lowPair1 = boost::assign::list_of // with one jack
    (Card(Card::Four, Suit::Spades))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Hearts));

const Card::List lowPair2 = boost::assign::list_of // with one three
    (Card(Card::Three, Suit::Diamonds))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Seven, Suit::Hearts));

const Card::List lowPair3 = boost::assign::list_of // with deuces
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Seven, Suit::Hearts));

// not a low pair
const Card::List notALowPair1 = boost::assign::list_of // with one queen
    (Card(Card::Four, Suit::Spades))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List notALowPair2 = boost::assign::list_of // with aces
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Seven, Suit::Hearts));

// middle pair
const Card::List middlePair1 = boost::assign::list_of // with one queen
    (Card(Card::Four, Suit::Spades))
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List middlePair2 = boost::assign::list_of // with one five
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Seven, Suit::Hearts));

const Card::List middlePair3 = boost::assign::list_of // with pair of sevens
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Eight, Suit::Hearts));

// not a middle pair
const Card::List notAMiddlePair1 = boost::assign::list_of // with 2 5
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List notAMiddlePair2 = boost::assign::list_of // with one 3
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Seven, Suit::Hearts));

// top pair
const Card::List topPair1 = boost::assign::list_of // with one king
    (Card(Card::Four, Suit::Spades))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List topPair2 = boost::assign::list_of // with one seven
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Seven, Suit::Hearts));

const Card::List topPair3 = boost::assign::list_of // with aces
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Eight, Suit::Hearts));

// not a top pair
const Card::List notATopPair1 = boost::assign::list_of // with 2 5
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List notATopPair2 = boost::assign::list_of // with one 3
    (Card(Card::Three, Suit::Diamonds))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Seven, Suit::Hearts));

// two pairs
const Card::List twoPairs1 = boost::assign::list_of // with 2 5
    (Card(Card::Four, Suit::Spades))
    (Card(Card::Four, Suit::Clubs))
    (Card(Card::Two, Suit::Spades))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List twoPairs2 = boost::assign::list_of // with 3 9
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Nine, Suit::Hearts));

const Card::List twoPairs3 = boost::assign::list_of // with aces
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Three, Suit::Hearts))
    (Card(Card::Five, Suit::Spades))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Eight, Suit::Hearts));

const Card::List twoPairs4 = boost::assign::list_of // with 2 5
    (Card(Card::Seven, Suit::Spades))
    (Card(Card::Seven, Suit::Clubs))
    (Card(Card::Two, Suit::Spades))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

// not a two pairs
const Card::List notATwoPairs1 = boost::assign::list_of // with 2 5
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::Queen, Suit::Hearts))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List notATwoPairs2 = boost::assign::list_of // with aces
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Seven, Suit::Hearts));

// three of kind
const Card::List threeOfKind1 = boost::assign::list_of // with 2 5
    (Card(Card::Two, Suit::Spades))
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Jack, Suit::Diamonds))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List threeOfKind2 = boost::assign::list_of // with 3 9
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Seven, Suit::Hearts));

const Card::List threeOfKind3 = boost::assign::list_of // with aces
    (Card(Card::Ace, Suit::Diamonds))
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Eight, Suit::Hearts));

// not a three of kind
const Card::List notAThreeOfKind1 = boost::assign::list_of // with 2 5
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::Jack, Suit::Hearts))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List notAThreeOfKind2 = boost::assign::list_of // with aces
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Three, Suit::Hearts));

// straight
const Card::List straight1 = boost::assign::list_of // with 2 5
    (Card(Card::Ace, Suit::Spades))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Four, Suit::Clubs));

const Card::List straight2 = boost::assign::list_of // with 3 9
    (Card(Card::Seven, Suit::Spades))
    (Card(Card::Eight, Suit::Clubs))
    (Card(Card::Three, Suit::Diamonds))
    (Card(Card::Ten, Suit::Clubs))
    (Card(Card::Jack, Suit::Diamonds));

const Card::List straight3 = boost::assign::list_of // with aces
    (Card(Card::King, Suit::Spades))
    (Card(Card::Queen, Suit::Hearts))
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Ten, Suit::Clubs));

const Card::List straight4 = boost::assign::list_of // with nine
    (Card(Card::King, Suit::Spades))
    (Card(Card::Queen, Suit::Hearts))
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Ten, Suit::Clubs));

// not a straight
const Card::List notAStraight1 = boost::assign::list_of // with 2 5
    (Card(Card::Ace, Suit::Spades))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::Ten, Suit::Hearts))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Clubs));

const Card::List notAStraight2 = boost::assign::list_of // with aces
    (Card(Card::King, Suit::Spades))
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Queen, Suit::Spades))
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Three, Suit::Hearts));

// flush
const Card::List flush1 = boost::assign::list_of // with 2 5
    (Card(Card::Ten, Suit::Clubs))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Four, Suit::Clubs));

const Card::List flush2 = boost::assign::list_of // with 3 10
    (Card(Card::Seven, Suit::Spades))
    (Card(Card::Eight, Suit::Spades))
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Ten, Suit::Clubs))
    (Card(Card::Jack, Suit::Spades));

const Card::List flush3 = boost::assign::list_of // with aces
    (Card(Card::King, Suit::Spades))
    (Card(Card::Queen, Suit::Spades))
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Three, Suit::Spades));

// not a flush
const Card::List notAFlush1 = boost::assign::list_of // with 2 5
    (Card(Card::Ace, Suit::Spades))
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Ten, Suit::Spades))
    (Card(Card::King, Suit::Spades))
    (Card(Card::Queen, Suit::Spades));

const Card::List notAFlush2 = boost::assign::list_of // with aces
    (Card(Card::King, Suit::Spades))
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Queen, Suit::Spades))
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Three, Suit::Hearts));

// full house
const Card::List fullhouse1 = boost::assign::list_of // with 2 5
    (Card(Card::Two, Suit::Spades))
    (Card(Card::Two, Suit::Hearts))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Four, Suit::Clubs));

const Card::List fullhouse2 = boost::assign::list_of // with 3 9
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Nine, Suit::Hearts))
    (Card(Card::Three, Suit::Hearts));

const Card::List fullhouse3 = boost::assign::list_of // with aces
    (Card(Card::Ace, Suit::Diamonds))
    (Card(Card::Ten, Suit::Spades))
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Ten, Suit::Clubs));

// not a full house
const Card::List notAfullhouse1 = boost::assign::list_of // with 2 5
    (Card(Card::Ace, Suit::Spades))
    (Card(Card::Ace, Suit::Clubs))
    (Card(Card::Ten, Suit::Spades))
    (Card(Card::Ten, Suit::Hearts))
    (Card(Card::Ten, Suit::Diamonds));

const Card::List notAfullhouse2 = boost::assign::list_of // with aces
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::Queen, Suit::Spades))
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Queen, Suit::Hearts));

// four of a kind
const Card::List fourOfKind1 = boost::assign::list_of // with 2 5
    (Card(Card::Two, Suit::Spades))
    (Card(Card::Two, Suit::Hearts))
    (Card(Card::Two, Suit::Diamonds))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Four, Suit::Clubs));

const Card::List fourOfKind2 = boost::assign::list_of // with 3 9
    (Card(Card::Three, Suit::Spades))
    (Card(Card::Three, Suit::Diamonds))
    (Card(Card::Three, Suit::Hearts));

const Card::List fourOfKind3 = boost::assign::list_of // with aces
    (Card(Card::Ace, Suit::Hearts))
    (Card(Card::Ace, Suit::Diamonds))
    (Card(Card::Ten, Suit::Spades));

// not a four of a kind
const Card::List notAFourOfKind1 = boost::assign::list_of // with 2 5
    (Card(Card::Ace, Suit::Spades))
    (Card(Card::Ten, Suit::Clubs))
    (Card(Card::Ten, Suit::Spades))
    (Card(Card::Ten, Suit::Hearts))
    (Card(Card::Ten, Suit::Diamonds));

const Card::List notAFourOfKind2 = boost::assign::list_of // with aces
    (Card(Card::Ace, Suit::Hearts))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Spades))
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Queen, Suit::Hearts));

// straight flush
const Card::List straightFlush1 = boost::assign::list_of // with 2 5
    (Card(Card::Six, Suit::Clubs))
    (Card(Card::Two, Suit::Hearts))
    (Card(Card::Two, Suit::Diamonds))
    (Card(Card::Three, Suit::Clubs))
    (Card(Card::Four, Suit::Clubs));

const Card::List straightFlush2 = boost::assign::list_of // with 3 9
    (Card(Card::Eight, Suit::Spades))
    (Card(Card::Ten, Suit::Spades))
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Queen, Suit::Spades))
    (Card(Card::Three, Suit::Spades));

const Card::List straightFlush3 = boost::assign::list_of // with aces
    (Card(Card::King, Suit::Spades))
    (Card(Card::Queen, Suit::Spades))
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Ace, Suit::Diamonds))
    (Card(Card::Ten, Suit::Spades));

// not a straight flush
const Card::List notAStraightFlush1 = boost::assign::list_of // with 2 5
    (Card(Card::Ace, Suit::Clubs))
    (Card(Card::Ten, Suit::Clubs))
    (Card(Card::Four, Suit::Clubs))
    (Card(Card::Three, Suit::Hearts))
    (Card(Card::Ace, Suit::Diamonds));

const Card::List notAStraightFlush2 = boost::assign::list_of // with aces
    (Card(Card::Ace, Suit::Hearts))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Queen, Suit::Spades))
    (Card(Card::Two, Suit::Clubs))
    (Card(Card::Queen, Suit::Hearts));

// top kicker
const Card::List topKicker1 = boost::assign::list_of // with A 2 notConnectors1
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Two, Suit::Hearts))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Seven, Suit::Clubs))
    (Card(Card::Four, Suit::Clubs));

const Card::List topKicker2 = boost::assign::list_of // with A K notConnectors2
    (Card(Card::King, Suit::Clubs))
    (Card(Card::Ten, Suit::Clubs))
    (Card(Card::Two, Suit::Spades))
    (Card(Card::Queen, Suit::Spades))
    (Card(Card::Three, Suit::Diamonds));

const Card::List topKicker3 = boost::assign::list_of // with A K notConnectors2
    (Card(Card::Ace, Suit::Clubs))
    (Card(Card::Two, Suit::Hearts))
    (Card(Card::Five, Suit::Spades))
    (Card(Card::Queen, Suit::Spades))
    (Card(Card::Three, Suit::Diamonds));

const Card::List topKicker4 = boost::assign::list_of // with A 3 aceAndSmall
    (Card(Card::King, Suit::Clubs))
    (Card(Card::Queen, Suit::Diamonds))
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Three, Suit::Diamonds))
    (Card(Card::Six, Suit::Hearts));

const Card::List topKicker5 = boost::assign::list_of // with A K notConnectors2
    (Card(Card::King, Suit::Clubs))
    (Card(Card::King, Suit::Diamonds))
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Three, Suit::Diamonds))
    (Card(Card::Six, Suit::Hearts));

// good kicker
const Card::List goodKicker1 = boost::assign::list_of // with Q J connectors2
    (Card(Card::Five, Suit::Clubs))
    (Card(Card::Two, Suit::Hearts))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Four, Suit::Clubs))
    (Card(Card::Four, Suit::Hearts));

const Card::List goodKicker2 = boost::assign::list_of // with 2 J oneHigh1
    (Card(Card::Queen, Suit::Clubs))
    (Card(Card::Ten, Suit::Spades))
    (Card(Card::Two, Suit::Diamonds))
    (Card(Card::Queen, Suit::Diamonds))
    (Card(Card::Three, Suit::Spades));

const Card::List goodKicker3 = boost::assign::list_of // with Q 3 oneHigh2
    (Card(Card::Ace, Suit::Spades))
    (Card(Card::Two, Suit::Hearts))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Three, Suit::Diamonds));

const Card::List goodKicker4 = boost::assign::list_of // with A J bothHigh1
    (Card(Card::Ace, Suit::Spades))
    (Card(Card::Queen, Suit::Hearts))
    (Card(Card::Five, Suit::Spades))
    (Card(Card::Three, Suit::Diamonds))
    (Card(Card::Ten, Suit::Spades));

// low kicker
const Card::List lowKicker1 = boost::assign::list_of  // with A 2 notConnectors1
    (Card(Card::Ace, Suit::Clubs))
    (Card(Card::Six, Suit::Hearts))
    (Card(Card::Five, Suit::Diamonds))
    (Card(Card::Jack, Suit::Clubs))
    (Card(Card::Four, Suit::Clubs));

const Card::List lowKicker2 = boost::assign::list_of // with 2 J oneHigh1
    (Card(Card::Jack, Suit::Spades))
    (Card(Card::Ten, Suit::Spades))
    (Card(Card::Three, Suit::Spades));

const Card::List lowKicker3 = boost::assign::list_of // with Q 3 oneHigh2
    (Card(Card::Ace, Suit::Spades))
    (Card(Card::Queen, Suit::Spades))
    (Card(Card::Three, Suit::Spades));

const Card::List lowKicker4 = boost::assign::list_of // with 2 5
    (Card(Card::Ace, Suit::Spades))
    (Card(Card::Queen, Suit::Spades))
    (Card(Card::Two, Suit::Diamonds))
    (Card(Card::Ten, Suit::Diamonds));


INSTANTIATE_TEST_CASE_P
(
    PoketHands,
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
        HandsTestParams(notSuited1, emptyBoard, Hand::PoketPair, false)
    )
);

INSTANTIATE_TEST_CASE_P
(
    Draws,
    HandsTest,
    Values
    (
        // straight draw
        HandsTestParams(connectors1, straightDraw1, Hand::StraightDraw, true),
        HandsTestParams(connectors2, straightDraw2, Hand::StraightDraw, true),
        HandsTestParams(connectors3, straightDraw3, Hand::StraightDraw, true),
        HandsTestParams(connectors1, straightDraw1, Hand::GoodDraw, false),
        HandsTestParams(connectors3, straightDraw3, Hand::TopDraw, true),

        HandsTestParams(connectors1, notAStraightDraw1, Hand::StraightDraw, false),
        HandsTestParams(connectors2, notAStraightDraw2, Hand::StraightDraw, false),
        HandsTestParams(connectors3, notAStraightDraw3, Hand::StraightDraw, false),
        HandsTestParams(aces, notAStraightDraw4, Hand::StraightDraw, false),
        HandsTestParams(aceKing, notAStraightDraw5, Hand::StraightDraw, false),
        HandsTestParams(aceKing, notAStraightDraw6, Hand::StraightDraw, false),

        // god shot
        HandsTestParams(trash, godShot1, Hand::GodShot, true),
        HandsTestParams(suited2, godShot2, Hand::GodShot, true),
        HandsTestParams(aces, godShot3, Hand::GodShot, true),
        HandsTestParams(aces, godShot4, Hand::GodShot, true),
        HandsTestParams(aces, godShot5, Hand::GodShot, true),

        // god shot
        HandsTestParams(trash, notAGodShot1, Hand::GodShot, false),
        HandsTestParams(suited2, notAGodShot2, Hand::GodShot, false),
        HandsTestParams(aces, notAGodShot3, Hand::GodShot, false),
        HandsTestParams(aces, notAGodShot4, Hand::GodShot, false),
        HandsTestParams(aces, notAGodShot5, Hand::GodShot, false),

        // flush draw
        HandsTestParams(suited1, flushDraw1, Hand::FlushDraw, true), // low
        HandsTestParams(suited1, flushDraw2, Hand::FlushDraw, true), // low
        HandsTestParams(suited2, flushDraw3, Hand::FlushDraw, true), // low
        HandsTestParams(notConnectors3, flushDraw4, Hand::FlushDraw, true), // low
        HandsTestParams(noBothHigh1, flushDraw5, Hand::FlushDraw, true), // middle
        HandsTestParams(aceSeven, flushDraw4, Hand::FlushDraw, true), // top
        HandsTestParams(suited1, notAFlushDraw1, Hand::FlushDraw, false),
        HandsTestParams(suited1, notAFlushDraw2, Hand::FlushDraw, false),
        HandsTestParams(diamonds, notAFlushDraw3, Hand::FlushDraw, false),

        HandsTestParams(noBothHigh1, flushDraw5, Hand::GoodDraw, true), // middle
        HandsTestParams(aceSeven, flushDraw4, Hand::TopDraw, true), // top
        HandsTestParams(suited1, flushDraw1, Hand::GoodDraw, false), // low
        HandsTestParams(suited1, flushDraw2, Hand::GoodDraw, false), // low
        HandsTestParams(suited2, flushDraw3, Hand::TopDraw, false), // low
        HandsTestParams(notConnectors3, flushDraw4, Hand::TopDraw, false) // low
    )
);

INSTANTIATE_TEST_CASE_P
(
    HighAndPair,
    HandsTest,
    Values
    (
        // high card
        HandsTestParams(oneHigh3, highCard1, Hand::HighCard, true),
        HandsTestParams(noBothHigh1, highCard2, Hand::HighCard, true),
        HandsTestParams(oneHigh3, notAHighCard1, Hand::HighCard, false),
        HandsTestParams(suited1, notAHighCard2, Hand::HighCard, false),

        // low pair
        HandsTestParams(bothHigh1, lowPair1, Hand::Pair, true),
        HandsTestParams(noOneHigh2, lowPair2, Hand::Pair, true),
        HandsTestParams(deuces, lowPair3, Hand::Pair, true),
        HandsTestParams(oneHigh2, notALowPair1, Hand::Pair, false),
        HandsTestParams(suited1, twoPairs1, Hand::Pair, false),

        HandsTestParams(bothHigh1, lowPair1, Hand::Low, true),
        HandsTestParams(noOneHigh2, lowPair2, Hand::Low, true),
        HandsTestParams(deuces, lowPair3, Hand::Low, true),
        HandsTestParams(oneHigh2, notALowPair1, Hand::Low, false),
        HandsTestParams(aces, notALowPair2, Hand::Low, false),
        HandsTestParams(suited1, twoPairs1, Hand::Low, false),

        // middle pair
        HandsTestParams(oneHigh2, middlePair1, Hand::Pair, true),
        HandsTestParams(suited1, middlePair2, Hand::Pair, true),
        HandsTestParams(sevens, middlePair3, Hand::Pair, true),
        HandsTestParams(suited1, notAMiddlePair1, Hand::Pair, false),
        HandsTestParams(notSuited2, twoPairs2, Hand::Pair, false),

        HandsTestParams(oneHigh2, middlePair1, Hand::Middle, true),
        HandsTestParams(suited1, middlePair2, Hand::Middle, true),
        HandsTestParams(sevens, middlePair3, Hand::Middle, true),
        HandsTestParams(suited1, notAMiddlePair1, Hand::Middle, false),
        HandsTestParams(noOneHigh2, notAMiddlePair2, Hand::Middle, false),
        HandsTestParams(notSuited2, twoPairs2, Hand::Middle, false),

        // top pair
        HandsTestParams(noBothHigh1, topPair1, Hand::Pair, true),
        HandsTestParams(aceSeven, topPair2, Hand::Pair, true),
        HandsTestParams(aces, topPair3, Hand::Pair, true),
        HandsTestParams(suited1, notATopPair1, Hand::Pair, false),
        HandsTestParams(aces, twoPairs3, Hand::Pair, false),

        HandsTestParams(noBothHigh1, topPair1, Hand::Top, true),
        HandsTestParams(aceSeven, topPair2, Hand::Top, true),
        HandsTestParams(aces, topPair3, Hand::Top, true),
        HandsTestParams(suited1, notATopPair1, Hand::Top, false),
        HandsTestParams(noOneHigh2, notATopPair2, Hand::Top, false)
    )
);

INSTANTIATE_TEST_CASE_P
(
    MiddleHands,
    HandsTest,
    Values
    (
        // two pairs
        HandsTestParams(suited1, twoPairs4, Hand::TwoPairs, true), // low
        HandsTestParams(suited1, twoPairs1, Hand::TwoPairs, true), // middle
        HandsTestParams(notSuited2, twoPairs2, Hand::TwoPairs, true), // top
        HandsTestParams(aces, twoPairs3, Hand::TwoPairs, true), // top
        HandsTestParams(suited1, notATwoPairs1, Hand::TwoPairs, false),
        HandsTestParams(aces, notATwoPairs2, Hand::TwoPairs, false),

        HandsTestParams(suited1, twoPairs4, Hand::Low, true), // low
        HandsTestParams(suited1, twoPairs1, Hand::Middle, true), // middle
        HandsTestParams(notSuited2, twoPairs2, Hand::Top, true), // top
        HandsTestParams(aces, twoPairs3, Hand::Top, true), // top

        // three of kind
        HandsTestParams(suited1, threeOfKind1, Hand::ThreeOfKind, true), // low
        HandsTestParams(notSuited2, threeOfKind2, Hand::ThreeOfKind, true), // middle
        HandsTestParams(aces, threeOfKind3, Hand::ThreeOfKind, true), // top
        HandsTestParams(suited1, notAThreeOfKind1, Hand::ThreeOfKind, false),
        HandsTestParams(aces, notAThreeOfKind2, Hand::ThreeOfKind, false),

        HandsTestParams(suited1, threeOfKind1, Hand::Low, true), // low
        HandsTestParams(notSuited2, threeOfKind2, Hand::Middle, true), // middle
        HandsTestParams(aces, threeOfKind3, Hand::Top, true), // top

        // straight
        HandsTestParams(suited1, straight1, Hand::Straight, true), // top
        HandsTestParams(notSuited2, straight2, Hand::Straight, true), // middle
        HandsTestParams(aces, straight3, Hand::Straight, true), // top
        HandsTestParams(notSuited2, straight4, Hand::Straight, true), // low
        HandsTestParams(suited1, notAStraight1, Hand::Straight, false),
        HandsTestParams(aces, notAStraight2, Hand::Straight, false),

        HandsTestParams(suited1, straight1, Hand::Top, true), // top
        HandsTestParams(notSuited2, straight2, Hand::Middle, true), // middle
        HandsTestParams(aces, straight3, Hand::Top, true), // top
        HandsTestParams(notSuited2, straight4, Hand::Low, true), // low

        // flush
        HandsTestParams(suited1, flush1, Hand::Flush, true), // low
        HandsTestParams(notSuited3, flush2, Hand::Flush, true), // middle
        HandsTestParams(aces, flush3, Hand::Flush, true), // top
        HandsTestParams(suited1, notAFlush1, Hand::Flush, false),
        HandsTestParams(aces, notAFlush2, Hand::Flush, false),

        HandsTestParams(suited1, flush1, Hand::Low, true), // low
        HandsTestParams(notSuited3, flush2, Hand::Middle, true), // middle
        HandsTestParams(aces, flush3, Hand::Top, true) // top
    )
);

INSTANTIATE_TEST_CASE_P
(
    TopHands,
    HandsTest,
    Values
    (
        // full house
        HandsTestParams(suited1, fullhouse1, Hand::FullHouse, true),
        HandsTestParams(notSuited2, fullhouse2, Hand::FullHouse, true),
        HandsTestParams(aces, fullhouse3, Hand::FullHouse, true),
        HandsTestParams(suited1, notAfullhouse1, Hand::FullHouse, false),
        HandsTestParams(aces, notAfullhouse2, Hand::FullHouse, false),

        // four of a kind
        HandsTestParams(suited1, fourOfKind1, Hand::FourOfKind, true),
        HandsTestParams(notSuited2, fourOfKind2, Hand::FourOfKind, true),
        HandsTestParams(aces, fourOfKind3, Hand::FourOfKind, true),
        HandsTestParams(suited1, notAFourOfKind1, Hand::FourOfKind, false),
        HandsTestParams(aces, notAFourOfKind2, Hand::FourOfKind, false),

        // straight flush
        HandsTestParams(suited1, straightFlush1, Hand::StraightFlush, true),
        HandsTestParams(notSuited2, straightFlush2, Hand::StraightFlush, true),
        HandsTestParams(aces, straightFlush3, Hand::StraightFlush, true),
        HandsTestParams(suited1, notAStraightFlush1, Hand::StraightFlush, false),
        HandsTestParams(aces, notAStraightFlush2, Hand::StraightFlush, false),

        // top kicker
        HandsTestParams(notConnectors1, topKicker1, Hand::TopKicker, true),
        HandsTestParams(notConnectors2, topKicker2, Hand::TopKicker, true),
        HandsTestParams(notConnectors2, topKicker3, Hand::TopKicker, true),
        HandsTestParams(aceAndSmall, topKicker4, Hand::TopKicker, true),
        HandsTestParams(notConnectors2, topKicker5, Hand::TopKicker, true), // top kicker
        HandsTestParams(connectors2, goodKicker1, Hand::TopKicker, false),
        HandsTestParams(oneHigh1, goodKicker2, Hand::TopKicker, false),
        HandsTestParams(notConnectors1, lowKicker1, Hand::TopKicker, false),
        HandsTestParams(oneHigh1, lowKicker2, Hand::TopKicker, false),

        // good kicker
        HandsTestParams(connectors2, goodKicker1, Hand::GoodKicker, true),
        HandsTestParams(oneHigh1, goodKicker2, Hand::GoodKicker, true),
        HandsTestParams(oneHigh2, goodKicker3, Hand::GoodKicker, true),
        HandsTestParams(bothHigh1, goodKicker4, Hand::GoodKicker, true),
        HandsTestParams(oneHigh2, threeOfKind2, Hand::GoodKicker, true), // good kicker
        HandsTestParams(notConnectors1, topKicker1, Hand::GoodKicker, false),
        HandsTestParams(notConnectors2, topKicker2, Hand::GoodKicker, false),
        HandsTestParams(oneHigh2, lowKicker3, Hand::GoodKicker, false),
        HandsTestParams(trash, lowKicker4, Hand::GoodKicker, false),

        // low kicker
        HandsTestParams(notConnectors1, lowKicker1, Hand::LowKicker, true),
        HandsTestParams(oneHigh1, lowKicker2, Hand::LowKicker, true),
        HandsTestParams(oneHigh2, lowKicker3, Hand::LowKicker, true),
        HandsTestParams(trash, lowKicker4, Hand::LowKicker, true),
        HandsTestParams(suited1, threeOfKind1, Hand::LowKicker, true), // low kicker
        HandsTestParams(oneHigh2, goodKicker3, Hand::LowKicker, false),
        HandsTestParams(bothHigh1, goodKicker4, Hand::LowKicker, false),
        HandsTestParams(notConnectors2, topKicker3, Hand::LowKicker, false),
        HandsTestParams(aceAndSmall, topKicker4, Hand::LowKicker, false)     
    )
);