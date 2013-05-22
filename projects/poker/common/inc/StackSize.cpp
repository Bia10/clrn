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

	StackSize::Value StackSize::FromValue(unsigned value, unsigned bigBlind, unsigned maxStack, unsigned minStack, unsigned pot, unsigned maxBet)
	{
		if (value < bigBlind * 10)
			return StackSize::Small;

        if (value < (pot - maxBet) / 2)
            return StackSize::Small;

        if (value < minStack * 3 / 2)
            return StackSize::Small;

        if (value > bigBlind * 20 || value > maxStack * 2 / 3)
            return StackSize::Big;

        return StackSize::Normal;
	}

};

