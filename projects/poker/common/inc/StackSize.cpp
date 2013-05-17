#include "StackSize.h"

namespace pcmn
{
	std::string StackSize::ToString(Value value)
	{
#define CASE(x) case x: return #x;
		switch (value)
		{
			CASE(Small)
			CASE(Normal)
			CASE(Big)
			default: return "unknown";
		}
#undef CASE
	}

	StackSize::Value StackSize::FromValue(unsigned value, unsigned bigBlind, unsigned maxStack, unsigned pot)
	{
		if (value < bigBlind * 10)
			return StackSize::Small;

        if (value < pot)
            return StackSize::Small;

		if (value < maxStack * 2)
			return StackSize::Normal;

		return StackSize::Big;
	}

};

