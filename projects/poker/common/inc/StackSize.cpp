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

	StackSize::Value StackSize::FromValue(unsigned value, unsigned bigBlind, unsigned maxStack, unsigned pot, unsigned maxBet)
	{
		if (value < bigBlind * 10)
			return StackSize::Small;

        if (value < (pot - maxBet) * 3 / 2)
            return StackSize::Small;

        if (value > bigBlind * 20)
            return StackSize::Big;

        return StackSize::Normal;
	}

};

