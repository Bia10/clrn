#include "Board.h"
#include "Conversion.h"

#include <gtest/gtest.h>

#include <boost/assign.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>

using testing::Range;
using testing::Combine;
using testing::Values;
using namespace pcmn;

typedef std::tuple<pcmn::Board::Value, bool, pcmn::Card::List> BoardTestParams;
unsigned g_Cnt = 0;

Card::List& Append(Card::List& l, const Card& c)
{
    l.push_back(c);
    return l;
}

class CardListHolder
{
public:
    CardListHolder(const Card& c) : m_List() 
    {
        m_List.push_back(c);
    }

    CardListHolder& operator () (const Card& c)
    {
        m_List.push_back(c);
        return *this;
    }

    operator const Card::List& () const
    {
        return m_List;
    }

private:
    Card::List m_List;
};

class BoardTest : public testing::TestWithParam<BoardTestParams>
{
public:

    void Do()
    {
        ++g_Cnt;

        const Board::Value value = std::get<0>(GetParam());
        const bool positive = std::get<1>(GetParam());
        const Card::List& board = std::get<2>(GetParam());

        Board b(board);
        b.Parse();

        const Board::Value result = b.GetValue();

        const std::vector<unsigned> toStore = conv::cast<std::vector<unsigned>>(static_cast<unsigned>(result));
        const Board::Value restored = static_cast<Board::Value>(conv::cast<unsigned>(toStore));

        EXPECT_EQ(result, restored);

        if (value == Board::Unknown)
        {
            if (positive)
                EXPECT_EQ(value, result) << "Expected that: '" << result << "' equals '" << value << "'";
            else
                EXPECT_NE(value, result) << "Expected that: '" << result << "' not equals '" << value << "'";
        }
        else
        {
            if (positive)
                EXPECT_TRUE(!!(result & value)) << "Expected that: '" << result << "' contains '" << value << "'";
            else
                EXPECT_FALSE(!!(result & value)) << "Expected that: '" << result << "' not contains '" << value << "'";
        }
    }
};

TEST_P(BoardTest, Parse)
{
    Do();
}

typedef BoardTestParams P;
typedef CardListHolder L;
typedef Card C;
typedef Suit S;

INSTANTIATE_TEST_CASE_P
(
    Boards1,
    BoardTest,
    Values
    (
        // unknown
        P(Board::Unknown, true, L(C(C::Two, S::Hearts))(C(C::Three, S::Diamonds))(C(C::Jack, S::Spades))),
        P(Board::Unknown, true, L(C(C::Queen, S::Spades))(C(C::Six, S::Hearts))(C(C::Two, S::Clubs))),

        P(Board::Unknown, false, L(C(C::King, S::Spades))(C(C::Three, S::Spades))(C(C::Six, S::Spades))),
        P(Board::Unknown, false, L(C(C::Nine, S::Spades))(C(C::Three, S::Spades))(C(C::Ace, S::Spades))),

        // low
        P(Board::Low, true, L(C(C::Two, S::Spades))(C(C::Three, S::Spades))(C(C::Six, S::Spades))),
        P(Board::Low, true, L(C(C::Nine, S::Spades))(C(C::Three, S::Spades))(C(C::Ten, S::Spades))),

        P(Board::Low, false, L(C(C::King, S::Spades))(C(C::Three, S::Spades))(C(C::Six, S::Spades))),
        P(Board::Low, false, L(C(C::Nine, S::Spades))(C(C::Three, S::Spades))(C(C::Ace, S::Spades))),

        // high
        P(Board::High, true, L(C(C::Ace, S::Spades))(C(C::King, S::Spades))(C(C::Jack, S::Spades))(C(C::Six, S::Spades))(C(C::Six, S::Hearts))),
        P(Board::High, true, L(C(C::Jack, S::Spades))(C(C::Queen, S::Spades))(C(C::King, S::Spades))(C(C::Six, S::Spades))),
        P(Board::High, true, L(C(C::Jack, S::Spades))(C(C::Queen, S::Spades))(C(C::Six, S::Spades))),

        P(Board::High, false, L(C(C::Two, S::Spades))(C(C::Three, S::Spades))(C(C::Seven, S::Spades))(C(C::Six, S::Spades))(C(C::Six, S::Hearts))),
        P(Board::High, false, L(C(C::Jack, S::Spades))(C(C::Queen, S::Spades))(C(C::Seven, S::Spades))(C(C::Six, S::Spades))),
        P(Board::High, false, L(C(C::Two, S::Spades))(C(C::Ace, S::Spades))(C(C::Six, S::Spades))),

        // ace
        P(Board::Ace, true, L(C(C::Ace, S::Spades))(C(C::King, S::Spades))(C(C::Jack, S::Spades))(C(C::Six, S::Spades))(C(C::Six, S::Hearts))),
        P(Board::Ace, true, L(C(C::Jack, S::Spades))(C(C::Queen, S::Spades))(C(C::Ace, S::Spades))(C(C::Six, S::Spades))),
        P(Board::Ace, true, L(C(C::Two, S::Spades))(C(C::Three, S::Spades))(C(C::Ace, S::Spades))),

        P(Board::Ace, false, L(C(C::Two, S::Spades))(C(C::Three, S::Spades))(C(C::Seven, S::Spades))(C(C::Six, S::Spades))(C(C::Six, S::Hearts))),
        P(Board::Ace, false, L(C(C::Jack, S::Spades))(C(C::Queen, S::Spades))(C(C::Seven, S::Spades))(C(C::Six, S::Spades))),
        P(Board::Ace, false, L(C(C::Two, S::Spades))(C(C::King, S::Spades))(C(C::Six, S::Spades))),
        
        // straight draw
        P(Board::StraightDraw, true, L(C(C::Ace, S::Spades))(C(C::King, S::Spades))(C(C::Jack, S::Spades))(C(C::Six, S::Spades))(C(C::Six, S::Hearts))),
        P(Board::StraightDraw, true, L(C(C::Jack, S::Spades))(C(C::Queen, S::Spades))(C(C::Ace, S::Spades))(C(C::Six, S::Spades))),
        P(Board::StraightDraw, true, L(C(C::Two, S::Spades))(C(C::Three, S::Spades))(C(C::Ace, S::Spades))),
        P(Board::StraightDraw, true, L(C(C::Ace, S::Spades))(C(C::King, S::Spades))(C(C::Five, S::Spades))(C(C::Seven, S::Spades))(C(C::Six, S::Hearts))),
        P(Board::StraightDraw, true, L(C(C::Jack, S::Spades))(C(C::Nine, S::Spades))(C(C::Seven, S::Spades))(C(C::Two, S::Spades))),
        P(Board::StraightDraw, true, L(C(C::Ace, S::Spades))(C(C::Three, S::Spades))(C(C::Four, S::Spades))),
        P(Board::StraightDraw, true, L(C(C::Ace, S::Spades))(C(C::Three, S::Spades))(C(C::Four, S::Spades))(C(C::Ace, S::Hearts))(C(C::Ace, S::Clubs))),

        P(Board::StraightDraw, false, L(C(C::Two, S::Spades))(C(C::Three, S::Spades))(C(C::Seven, S::Spades))(C(C::Eight, S::Spades))(C(C::Eight, S::Hearts))),
        P(Board::StraightDraw, false, L(C(C::Jack, S::Spades))(C(C::Queen, S::Spades))(C(C::Seven, S::Spades))(C(C::Six, S::Spades))),
        P(Board::StraightDraw, false, L(C(C::Two, S::Spades))(C(C::King, S::Spades))(C(C::Six, S::Spades))),
        P(Board::StraightDraw, false, L(C(C::Ace, S::Spades))(C(C::King, S::Spades))(C(C::Jack, S::Spades))(C(C::Ten, S::Spades))(C(C::Six, S::Hearts))),
        P(Board::StraightDraw, false, L(C(C::Jack, S::Spades))(C(C::Queen, S::Spades))(C(C::Ace, S::Spades))(C(C::King, S::Spades))),

        // straight
        P(Board::Straight, true, L(C(C::Ace, S::Spades))(C(C::King, S::Spades))(C(C::Jack, S::Spades))(C(C::Ten, S::Spades))(C(C::Six, S::Hearts))),
        P(Board::Straight, true, L(C(C::Jack, S::Spades))(C(C::Queen, S::Spades))(C(C::Ace, S::Spades))(C(C::King, S::Spades))),
        P(Board::Straight, true, L(C(C::Two, S::Spades))(C(C::Four, S::Spades))(C(C::Five, S::Spades))(C(C::Eight, S::Spades))(C(C::Six, S::Hearts))),
        P(Board::Straight, true, L(C(C::Jack, S::Spades))(C(C::Nine, S::Spades))(C(C::Seven, S::Spades))(C(C::Eight, S::Spades))),
        P(Board::Straight, true, L(C(C::Ace, S::Spades))(C(C::Three, S::Spades))(C(C::Four, S::Spades))(C(C::Five, S::Hearts))(C(C::Ace, S::Clubs))),

        P(Board::Straight, false, L(C(C::Ace, S::Spades))(C(C::Three, S::Spades))(C(C::Four, S::Spades))),
        P(Board::Straight, false, L(C(C::Ace, S::Spades))(C(C::Three, S::Spades))(C(C::Four, S::Spades))(C(C::Ace, S::Hearts))(C(C::Ace, S::Clubs))),
        P(Board::Straight, false, L(C(C::Two, S::Spades))(C(C::Three, S::Spades))(C(C::Seven, S::Spades))(C(C::Eight, S::Spades))(C(C::Eight, S::Hearts))),
        P(Board::Straight, false, L(C(C::Jack, S::Spades))(C(C::Queen, S::Spades))(C(C::Seven, S::Spades))(C(C::Six, S::Spades))),
        P(Board::Straight, false, L(C(C::Two, S::Spades))(C(C::King, S::Spades))(C(C::Six, S::Spades))),

        // flush draw
        P(Board::FlushDraw, true, L(C(C::Ace, S::Spades))(C(C::King, S::Spades))(C(C::Jack, S::Spades))(C(C::Six, S::Hearts))(C(C::Six, S::Hearts))),
        P(Board::FlushDraw, true, L(C(C::Jack, S::Hearts))(C(C::Queen, S::Hearts))(C(C::Ace, S::Spades))(C(C::Six, S::Hearts))),
        P(Board::FlushDraw, true, L(C(C::Two, S::Spades))(C(C::Three, S::Spades))(C(C::Ace, S::Spades))),

        P(Board::FlushDraw, false, L(C(C::Two, S::Spades))(C(C::Three, S::Spades))(C(C::Seven, S::Spades))(C(C::Six, S::Spades))(C(C::Six, S::Hearts))),
        P(Board::FlushDraw, false, L(C(C::Jack, S::Spades))(C(C::Queen, S::Spades))(C(C::Seven, S::Hearts))(C(C::Six, S::Hearts))),
        P(Board::FlushDraw, false, L(C(C::Two, S::Spades))(C(C::King, S::Clubs))(C(C::Six, S::Spades)))
    )
);

INSTANTIATE_TEST_CASE_P
(
    Boards2,
    BoardTest,
    Values
    (
        // flush
        P(Board::Flush, true, L(C(C::Ace, S::Spades))(C(C::King, S::Spades))(C(C::Jack, S::Spades))(C(C::Six, S::Spades))(C(C::Six, S::Spades))),
        P(Board::Flush, true, L(C(C::Jack, S::Spades))(C(C::Queen, S::Spades))(C(C::Ace, S::Spades))(C(C::Six, S::Spades))),

        P(Board::Flush, false, L(C(C::Two, S::Spades))(C(C::Three, S::Spades))(C(C::Seven, S::Spades))(C(C::Six, S::Hearts))(C(C::Six, S::Hearts))),
        P(Board::Flush, false, L(C(C::Jack, S::Spades))(C(C::Queen, S::Spades))(C(C::Seven, S::Hearts))(C(C::Six, S::Hearts))),
        P(Board::Flush, false, L(C(C::Two, S::Spades))(C(C::King, S::Clubs))(C(C::Six, S::Spades))),

        // pair
        P(Board::Pair, true, L(C(C::Ace, S::Spades))(C(C::King, S::Spades))(C(C::Jack, S::Spades))(C(C::Six, S::Spades))(C(C::Six, S::Spades))),
        P(Board::Pair, true, L(C(C::Ace, S::Spades))(C(C::Ace, S::Hearts))(C(C::Jack, S::Spades))(C(C::Six, S::Spades))(C(C::Six, S::Spades))),
        P(Board::Pair, true, L(C(C::Jack, S::Spades))(C(C::Jack, S::Diamonds))(C(C::Jack, S::Hearts))(C(C::Jack, S::Clubs))),
        P(Board::Pair, true, L(C(C::Jack, S::Spades))(C(C::Jack, S::Diamonds))(C(C::Queen, S::Hearts))(C(C::Queen, S::Clubs))),

        P(Board::Pair, false, L(C(C::Two, S::Spades))(C(C::Three, S::Spades))(C(C::Seven, S::Spades))(C(C::Six, S::Hearts))(C(C::Eight, S::Hearts))),
        P(Board::Pair, false, L(C(C::Jack, S::Spades))(C(C::Queen, S::Spades))(C(C::Seven, S::Hearts))(C(C::Six, S::Hearts))),
        P(Board::Pair, false, L(C(C::Two, S::Spades))(C(C::King, S::Clubs))(C(C::Six, S::Spades)))
    )
);