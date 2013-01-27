#include "CombinationsCalculator.h"
#include "../../evaluator/HandEval.h"
#include "../../evaluator/SevenEval.h"

#include "gtest/gtest.h"

#include <iostream>

#include <boost/assign.hpp>

using testing::Range;
using testing::Combine;
using testing::Values;
using namespace pcmn;

typedef ::std::tr1::tuple<int, int, int, float>  ParamsWithoutFlop;
 
typedef ::std::tr1::tuple<int, int, Card::List, int, float>  ParamsWithFlop;

class TestHands : public testing::TestWithParam<ParamsWithoutFlop>
{
public:

	void Do()
	{
		std::vector<short> ranges(::std::tr1::get<2>(GetParam()) - 1, Evaluator::CARD_DECK_SIZE);
		std::vector<int> flop;

		const float percents =  m_Calc.GetEquity(std::tr1::get<0>(GetParam()), std::tr1::get<1>(GetParam()), flop, ranges);

		ASSERT_TRUE(fabs(percents - std::tr1::get<3>(GetParam())) < 1.0f) << percents;
	}
private:
	Evaluator m_Calc;
};

class TestHandsAndFlop : public testing::TestWithParam<ParamsWithFlop>
{
public:

	void Do()
	{
		std::vector<short> ranges(::std::tr1::get<3>(GetParam()) - 1, Evaluator::CARD_DECK_SIZE);

		Card::List flopCards = std::tr1::get<2>(GetParam());

		std::vector<int> flop;
		for (const Card& card : flopCards)
			flop.push_back(card.ToEvalFormat());

		const float percents =  m_Calc.GetEquity(std::tr1::get<0>(GetParam()), std::tr1::get<1>(GetParam()), flop, ranges);

		ASSERT_TRUE(fabs(percents - std::tr1::get<4>(GetParam())) < 1.0f) << percents;
	}
private:
	Evaluator m_Calc;
};

TEST(Cards, Convertion)
{
	Evaluator ev;
	for (int i = 0 ; i < 1000; ++i)
	{
		bool dead[Evaluator::CARD_DECK_SIZE] = {false};
		const short evalValue = ev.GetRandomCard(dead);
		Card card;
		card.FromEvalFormat(evalValue);

		const short converted = card.ToEvalFormat();

		EXPECT_EQ(evalValue, converted);
	}
}

TEST(HandEval, Old)
{
	std::auto_ptr<HandEval> eval(new HandEval());
	int holeCards[4] = {0, 11, 37, 38}; // Two pairs of hole cards, the first
	// player has the Ace of Spades and the
	// Queen of Clubs, while the second
	// player has the 5 of Hearts and the
	// 5 of Diamonds.


	// The equity should be approximately 45.33% versus 54.67%.
	EXPECT_TRUE(fabs(45.33f - eval->computePreFlopEquityForSpecificHoleCards(holeCards, 2)) < 0.1f);
	eval->timeRankMethod();

	std::auto_ptr<SevenEval> sEval(new SevenEval());

	EXPECT_EQ(sEval->getRankOfSeven(0, 1, 2, 3, 4, 5, 6), 7452);// Ace quad with kings, 7452
	EXPECT_EQ(sEval->getRankOfSeven(5, 6, 2, 4, 3, 0, 1), 7452);// Shuffled ace quad with kings, again 7452
	EXPECT_EQ(sEval->getRankOfSeven(0, 4, 8, 12, 16, 20, 24), 7462);// Spade royal flush, 7462
	EXPECT_EQ(sEval->getRankOfSeven(51, 47, 43, 39, 30, 26, 22), 49);// 49, the worst hand.
}

TEST(Evaluator, Ranks)
{
	Evaluator calc;
	short straight;
	short flush;
	short pair;
	short highCard;
	short twoSmallPairs;
	short twoBigPairs;

	{
		Card::List cards = boost::assign::list_of
			(Card(Card::Ace, Suit::Clubs))
			(Card(Card::Two, Suit::Spades))
			(Card(Card::Three, Suit::Hearts))
			(Card(Card::Four, Suit::Clubs))
			(Card(Card::Five, Suit::Spades))
			(Card(Card::Queen, Suit::Clubs))
			(Card(Card::Nine, Suit::Clubs));

		straight = calc.GetRank(cards);
		const Combination::Value rank = Combination::FromEval(straight);
		EXPECT_EQ(rank, Combination::Staight);
	}

	{
		Card::List cards = boost::assign::list_of
			(Card(Card::Ace, Suit::Clubs))
			(Card(Card::Two, Suit::Spades))
			(Card(Card::Three, Suit::Clubs))
			(Card(Card::Four, Suit::Clubs))
			(Card(Card::Five, Suit::Spades))
			(Card(Card::Queen, Suit::Clubs))
			(Card(Card::Nine, Suit::Clubs));

		flush = calc.GetRank(cards);
		const Combination::Value rank = Combination::FromEval(flush);
		EXPECT_EQ(rank, Combination::Flush);
	}
	{
		Card::List cards = boost::assign::list_of
			(Card(Card::Ace, Suit::Clubs))
			(Card(Card::Ace, Suit::Spades))
			(Card(Card::Three, Suit::Hearts))
			(Card(Card::Four, Suit::Clubs))
			(Card(Card::Five, Suit::Spades))
			(Card(Card::Queen, Suit::Clubs))
			(Card(Card::Nine, Suit::Clubs));

		pair = calc.GetRank(cards);
		const Combination::Value rank = Combination::FromEval(pair);
		EXPECT_EQ(rank, Combination::Pair);
	}
	{
		Card::List cards = boost::assign::list_of
			(Card(Card::Ace, Suit::Clubs))
			(Card(Card::Two, Suit::Spades))
			(Card(Card::Jack, Suit::Hearts))
			(Card(Card::Four, Suit::Clubs))
			(Card(Card::Five, Suit::Spades))
			(Card(Card::Queen, Suit::Clubs))
			(Card(Card::Nine, Suit::Clubs));

		highCard = calc.GetRank(cards);
		const Combination::Value rank = Combination::FromEval(highCard);
		EXPECT_EQ(rank, Combination::HighCard);
	}
	{
		Card::List cards = boost::assign::list_of
			(Card(Card::Two, Suit::Clubs))
			(Card(Card::Two, Suit::Spades))
			(Card(Card::Three, Suit::Hearts))
			(Card(Card::Three, Suit::Clubs))
			(Card(Card::Four, Suit::Spades))
			(Card(Card::Five, Suit::Clubs))
			(Card(Card::Seven, Suit::Clubs));

		twoSmallPairs = calc.GetRank(cards);
		const Combination::Value rank = Combination::FromEval(twoSmallPairs);
		EXPECT_EQ(rank, Combination::TwoPairs);
	}
	{
		Card::List cards = boost::assign::list_of
			(Card(Card::Ace, Suit::Clubs))
			(Card(Card::Ace, Suit::Spades))
			(Card(Card::King, Suit::Hearts))
			(Card(Card::King, Suit::Clubs))
			(Card(Card::Five, Suit::Spades))
			(Card(Card::Queen, Suit::Clubs))
			(Card(Card::Nine, Suit::Clubs));

		twoBigPairs = calc.GetRank(cards);
		const Combination::Value rank = Combination::FromEval(twoBigPairs);
		EXPECT_EQ(rank, Combination::TwoPairs);
	}

	EXPECT_GT(flush, straight);
	EXPECT_GT(straight, twoBigPairs);
	EXPECT_GT(twoBigPairs, twoSmallPairs);
	EXPECT_GT(twoSmallPairs, pair);
	EXPECT_GT(pair, highCard);

}

TEST_P(TestHands, Simple)
{
	Do();	
}

TEST_P(TestHandsAndFlop, Simple)
{
	Do();	
}

INSTANTIATE_TEST_CASE_P
(
	Combined,
	TestHands,
	Values
	(
		ParamsWithoutFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), 2, 67.054f),
		ParamsWithoutFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), 3, 50.700f),
		ParamsWithoutFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), 4, 41.435f),
		ParamsWithoutFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), 5, 35.444f),
		ParamsWithoutFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), 6, 31.044f),
		ParamsWithoutFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), 7, 27.691f),
		ParamsWithoutFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), 8, 24.960f),
		ParamsWithoutFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), 9, 22.631f)

	)
);

Card::List flop =  boost::assign::list_of(Card(Card::Five, Suit::Hearts))(Card(Card::Two, Suit::Diamonds))(Card(Card::Eight, Suit::Clubs));

INSTANTIATE_TEST_CASE_P
(
	Combined,
	TestHandsAndFlop,
	Values
	(
		ParamsWithFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), flop, 2, 52.214f),
		ParamsWithFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), flop, 3, 31.435f),
		ParamsWithFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), flop, 4, 21.472f),
		ParamsWithFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), flop, 5, 16.189f),
		ParamsWithFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), flop, 6, 12.883f),
		ParamsWithFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), flop, 7, 10.697f),
		ParamsWithFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), flop, 8, 9.003f),
		ParamsWithFlop(Card(Card::Ace, Suit::Spades).ToEvalFormat(), Card(Card::King, Suit::Spades).ToEvalFormat(), flop, 9, 7.639f)
	)
);