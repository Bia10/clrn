#ifndef BetSize_h__
#define BetSize_h__

#include <string>

namespace pcmn
{
	struct BetSize
	{
		enum Value
		{
			VeryLow		= 0,
			Low			= 1,
			Normal		= 2,
			High		= 3,
			Huge		= 4,
			Max			= Huge
		};

		static std::string ToString(Value value);
        static Value FromParams
        (
            unsigned bet, 
            unsigned maxBet, 
            unsigned pot, 
            unsigned stack, 
            unsigned bigBlind
        );
	};

};

#endif // BetSize_h__
