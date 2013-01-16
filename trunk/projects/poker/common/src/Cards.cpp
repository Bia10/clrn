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
	return "";
}

clnt::Card::Value clnt::Card::FromString(const char value)
{
	switch (value)
	{
	case '2': return Two;
	case '3': return Three;
	case '4': return Four;
	case '5': return Five;
	case '6': return Six;
	case '7': return Seven;
	case '8': return Eight;
	case '9': return Nine;
	case 'T': return Ten;
	case 'J': return Jack;
	case 'Q': return Queen;
	case 'K': return King;
	case 'A': return Ace;
	default: assert(false); return Unknown;
	}
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
	return "";
}
