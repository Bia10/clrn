#include "Cards.h"
#include <cassert>

std::string pcmn::Card::ToString(Value value)
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

pcmn::Card::Value pcmn::Card::FromString(const char value)
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

short pcmn::Card::ToEvalFormat() const
{
	const int result = (Ace - m_Value) * 4;
	switch (m_Suit)
	{
		case Suit::Spades: return result;
		case Suit::Hearts: return result + 1;
		case Suit::Diamonds: return result + 2;
		case Suit::Clubs: return result + 3;
	}
	assert(false);
	return 0;
}

const pcmn::Card& pcmn::Card::FromEvalFormat(short value)
{
	m_Value = static_cast<Value>(Ace - value / 4);
	int rest = value % 4;
	switch (rest)
	{
	case 0: m_Suit = Suit::Spades; break;
	case 1: m_Suit =  Suit::Hearts; break;
	case 2: m_Suit =  Suit::Diamonds; break;
	case 3: m_Suit =  Suit::Clubs; break;
	}
	return *this;
}

std::string pcmn::Suit::ToString(Value value)
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
