#ifndef Combinations_h__
#define Combinations_h__

#include <string>

namespace clnt
{
	struct Combination
	{
		enum Value
		{
			Unknown			= 0,
			HighCard		= 1,
			Pair			= 2,
			TwoPairs		= 3,
			ThreeOfKind		= 4,
			Staight			= 5,
			Flush			= 6,
			FullHouse		= 7,
			FourOfKind		= 8,
			RoyalFlush		= 9
		};

		static std::string ToString(Value value);
		static Value FromEval(int value);
	};
}

#endif // Combinations_h__
