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

	StackSize::Value StackSize::FromValue(std::size_t value, std::size_t bigBlind, std::size_t maxStack)
	{
		if (value < bigBlind * 10)
			return StackSize::Small;

		if (value < maxStack * 2)
			return StackSize::Normal;
		return StackSize::Big;
	}

};

