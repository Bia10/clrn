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
		if (value < 20)
			return WinRate::VeryLow;
		if (value < 30)
			return WinRate::Low;
		if (value < 40)
			return WinRate::Normal;
		if (value < 50)
			return WinRate::Good;
		if (value < 80)
			return WinRate::VeryGood;
		
		return WinRate::Nuts;
	}

	float WinRate::ToValue(Value value)
	{
		switch (value)
		{
		case VeryLow: return 0.0f;
		case Low: return 0.2f;
		case Normal: return 0.3f;
		case Good: return 0.4f;
		case VeryGood: return 0.7f;
		case Nuts: return 1.0f;
		default: return 0;
		}
	}

};

