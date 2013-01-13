#include "Cards.h"
#include <cassert>

std::string clnt::Card::ToString(Value value)
{
#define CASE(x) case x: return #x;
	switch (value)
	{
		CASE(Two)
		CASE(Three)
		CASE(Four)
		CASE(Five)
		CASE(Six)
		CASE(Seven)
		CASE(Eight)
		CASE(Nine)
		CASE(Ten)
		CASE(Jack)
		CASE(Queen)
		CASE(King)
		CASE(Ace)
	default: assert(false);
	}
#undef CASE
}

std::string clnt::Suit::ToString(Value value)
{
#define CASE(x) case x: return #x;
	switch (value)
	{
		CASE(Hearts)
		CASE(Clubs)
		CASE(Spades)
		CASE(Diamonds)
	default: assert(false);
	}
#undef CASE
}
