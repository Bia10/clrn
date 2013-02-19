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
		static Value FromValue(float value, bool isPot);
	};

};

#endif // BetSize_h__
