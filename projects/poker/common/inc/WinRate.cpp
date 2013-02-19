#include "WinRate.h"

namespace pcmn
{
	std::string WinRate::ToString(Value value)
	{
#define CASE(x) case x: return #x;
		switch (value)
		{
			CASE(VeryLow)
			CASE(Low)
			CASE(Normal)
			CASE(Good)
			CASE(VeryGood)
			CASE(Nuts)
		default: return "unknown";
		}
#undef CASE
	}

	WinRate::Value WinRate::FromValue(float value)
	{
		if (value < 10)
			return WinRate::VeryLow;
		if (value < 20)
			return WinRate::Low;
		if (value < 40)
			return WinRate::Normal;
		if (value < 60)
			return WinRate::Good;
		if (value < 80)
			return WinRate::VeryGood;
		
		return WinRate::Nuts;
	}

};

