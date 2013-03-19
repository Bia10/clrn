#include "BetSize.h"

namespace pcmn
{
	std::string BetSize::ToString(Value value)
	{
#define CASE(x) case x: return #x;
		switch (value)
		{
			CASE(VeryLow)
			CASE(Low)
			CASE(Normal)
			CASE(High)
			CASE(Huge)
			default: return "unknown";
		}
#undef CASE
	}

	BetSize::Value BetSize::FromStack(unsigned bet, unsigned maxBet, unsigned stack)
	{
		const float value = stack ? static_cast<float>(maxBet - bet) / stack : 0;
		if (value < 0.05f)
			return BetSize::VeryLow;
		if (value < 0.1f)
			return BetSize::Low;
		if (value < 0.3f)
			return BetSize::Normal;
		if (value < 0.6f)
			return BetSize::High;

		return BetSize::Huge;
	}

	BetSize::Value BetSize::FromPot(unsigned bet, unsigned maxBet, unsigned pot)
	{
		const float value = static_cast<float>(maxBet - bet) / pot;

		if (value < 0.1f)
			return BetSize::VeryLow;
		if (value < 0.3f)
			return BetSize::Low;
		if (value < 0.6f)
			return BetSize::Normal;
		if (value < 2.0f)
			return BetSize::High;

		return BetSize::Huge;
	}

};

