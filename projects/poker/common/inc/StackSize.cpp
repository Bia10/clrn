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

        if (value < bigBlind * 30)
            return StackSize::Normal;

        if (value > bigBlind * 40)
            return StackSize::Big;

        const unsigned diff = maxStack - minStack;
        const unsigned medium = minStack + diff / 2;
        return (value > medium) ? StackSize::Big : StackSize::Small;
	}

};

