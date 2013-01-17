#include "Combinations.h"

#include <cassert>

namespace clnt
{
	
std::string Combination::ToString(Value value)
{
#define CASE(x) case x: return #x;
	switch (value)
	{
		CASE(HighCard)
		CASE(Pair)
		CASE(TwoPairs)
		CASE(ThreeOfKind)
		CASE(Staight)
		CASE(Flush)
		CASE(FullHouse)
		CASE(FourOfKind)
		CASE(RoyalFlush)
	default: assert(false);
	}
#undef CASE
	return "";
}

Combination::Value Combination::FromEval(int rank)
{
	if (rank < 1278) // high card 1277 - 49
		return Combination::HighCard;

	if (rank < 4138) // pair 4137 - 1296
		return Combination::Pair;

	if (rank < 4996) // two pairs 5855 - 4996
		return Combination::TwoPairs;

	if (rank < 5854) // set 5853 - 5004
		return Combination::ThreeOfKind;

	if (rank < 5864) // straight 5863 - 5857
		return Combination::Staight;

	if (rank < 7141) // flush 7140 - 5865
		return Combination::Flush;

	if (rank < 7297) // full 7296 - 7141
		return Combination::FullHouse;

	if (rank < 7453) // quad 7452 - 7299
		return Combination::FourOfKind;

	if (rank == 7462) // royal 7462
		return Combination::RoyalFlush;

	assert(false);
	return Combination::Unknown;
}

}
