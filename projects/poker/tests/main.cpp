#include "CombinationsCalculator.h"
#include "../../evaluator/HandEval.h"

#include <iostream>

#include <boost/assign.hpp>

using namespace clnt;

int main()
{
	HandEval *eval = new HandEval();
	int holeCards[4] = {0, 11, 37, 38}; // Two pairs of hole cards, the first
	// player has the Ace of Spades and the
	// Queen of Clubs, while the second
	// player has the 5 of Hearts and the
	// 5 of Diamonds.

	// The equity should be approximately 45.33% versus 54.67%.
	printf("\n%s\n", eval->computePreFlopEquityForSpecificHoleCards(holeCards, 1));

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
	return 0;
}