#ifndef WinRate_h__
#define WinRate_h__

#include <string>

namespace pcmn
{
	struct WinRate
	{
		enum Value
		{
			VeryLow		= 0,		//	(< 10)
			Low			= 1,  		// (10 - 20)	
			Normal		= 2,		// (20 - 40)
			Good		= 3, 		// (40 - 60)
			VeryGood	= 4,		// (60 - 80)
			Nuts		= 5,		// (80	- 100)
			Max			= Nuts
		};

		static std::string ToString(Value value);
		static Value FromValue(float value);
		static float ToValue(Value value);
	};

};

#endif // WinRate_h__
