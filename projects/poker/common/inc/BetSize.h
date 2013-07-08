#ifndef BetSize_h__
#define BetSize_h__

#include <string>

namespace pcmn
{
	struct BetSize
	{
		enum Value
		{
			NoBet		= 0,
			Low			= 1,
			Normal		= 2,
			High		= 3,
			Huge		= 4,
			Max			= Huge
		};

		static std::string ToString(Value value);
        static Value FromAction
        (
            unsigned bet, 
            unsigned pot, 
            unsigned stack, 
            unsigned bigBlind
        );

        static Value FromDecision
        (
            unsigned bet, 
            unsigned maxBet, 
            unsigned pot, 
            unsigned stack, 
            unsigned bigBlind
        );

        static unsigned FromValue
        (
            Value v, 
            unsigned pot, 
            unsigned stack, 
            unsigned alreadyBet,
            unsigned bigBlind
         );
	};

};

#endif // BetSize_h__
