#include "CombinationsCalculator.h"
#include "../../evaluator/HandEval.h"
#include "../../evaluator/SevenEval.h"
#include "../protocols/inc/packet.pb.h"

#include "gtest/gtest.h"

#include <iostream>

#include <boost/assign.hpp>

using namespace clnt;

TEST(HandEval, Old)
{
	packets::Packet packet;
	

	HandEval *eval = new HandEval();
	int holeCards[4] = {0, 11, 37, 38}; // Two pairs of hole cards, the first
	// player has the Ace of Spades and the
	// Queen of Clubs, while the second
	// player has the 5 of Hearts and the
	// 5 of Diamonds.

	// The equity should be approximately 45.33% versus 54.67%.
	printf("\n%s\n", eval->computePreFlopEquityForSpecificHoleCards(holeCards, 2));
	eval->timeRankMethod();
	delete eval;

	SevenEval *sEval = new SevenEval();
	printf("%i\n", sEval->getRankOfSeven(0, 1, 2, 3, 4, 5, 6)); // Ace quad with kings, 7452
	printf("%i\n", sEval->getRankOfSeven(5, 6, 2, 4, 3, 0, 1)); // Shuffled ace quad with kings, again 7452
	printf("%i\n", sEval->getRankOfSeven(0, 4, 8, 12, 16, 20, 24)); // Spade royal flush, 7462
	printf("%i\n", sEval->getRankOfSeven(51, 47, 43, 39, 30, 26, 22)); // 49, the worst hand.
	delete sEval;
}

TEST(Test, Sample)
{
	EXPECT_EQ(1, 1);
	HandEval *eval = new HandEval();
	int holeCards[4] = {0, 11, 37, 38}; // Two pairs of hole cards, the first
	// player has the Ace of Spades and the
	// Queen of Clubs, while the second
	// player has the 5 of Hearts and the
	// 5 of Diamonds.

	// The equity should be approximately 45.33% versus 54.67%.
	printf("\n%s\n", eval->computePreFlopEquityForSpecificHoleCards(holeCards, 2));

	Calculator calc;

	{
		Card::List cards = boost::assign::list_of
			(Card(Card::Ace, Suit::Clubs))
			(Card(Card::Two, Suit::Spades))
			(Card(Card::Three, Suit::Hearts))
			(Card(Card::Four, Suit::Clubs))
			(Card(Card::Five, Suit::Spades))
			(Card(Card::Queen, Suit::Clubs))
			(Card(Card::Nine, Suit::Clubs));

		const Combination::Value rank = Combination::FromEval(calc.GetRank(cards));
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

		const Combination::Value rank = Combination::FromEval(calc.GetRank(cards));
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

		const Combination::Value rank = Combination::FromEval(calc.GetRank(cards));
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

		const Combination::Value rank = Combination::FromEval(calc.GetRank(cards));
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

		const Combination::Value rank = Combination::FromEval(calc.GetRank(cards));
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

		const Combination::Value rank = Combination::FromEval(calc.GetRank(cards));
	}
}
