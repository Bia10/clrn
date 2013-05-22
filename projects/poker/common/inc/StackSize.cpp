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

        const unsigned diff = maxStack - minStack;

        if (diff < value / 2 && value > bigBlind * 20)
            return StackSize::Big;

        const unsigned medium = minStack + diff / 2;
        if (value > medium && value > bigBlind * 20)
            return StackSize::Big;

        const unsigned lowDiff = medium - minStack;
        const unsigned lowLimit = minStack + lowDiff / 2;
        if (value < lowLimit)
            return StackSize::Small;

        return StackSize::Normal;
	}

};

