#include "CombinationsCalculator.h"

#include <iostream>

#include <boost/assign.hpp>

using namespace clnt;

int main()
{
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