#ifndef BetSize_h__
#define BetSize_h__

#include <string>

namespace pcmn
{
	struct BetSize
	{
		enum Value
		{
			VeryLow		= 0,	//  (0.1 пота, 0.05 стека)
			Low			= 1,	// (0.1-0.3, 0.05-0.1)
			Normal		= 2, 	// (0.3-1.1, 0.1-0.3)
			High		= 3,	// (1.1 - 2, 0.3-0.6)
			Huge		= 4,	// (>2, >0.6)
			Max			= Huge
		};

		static std::string ToString(Value value);
		static float ToPot(Value value);
		static float ToStack(Value value);
        static Value FromPot(unsigned bet, unsigned maxBet, unsigned pot, float& value);
        static Value FromStack(unsigned bet, unsigned maxBet, unsigned stack, float& value);
        static Value FromPot(const float value);
        static Value FromStack(const float value);
	};

};

#endif // BetSize_h__
