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

	BetSize::Value BetSize::FromStack(unsigned bet, unsigned maxBet, unsigned stack, float& value)
	{
		value = stack ? static_cast<float>(maxBet - bet) / stack : 0;
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

	BetSize::Value BetSize::FromPot(unsigned bet, unsigned maxBet, unsigned pot, float& value)
	{
		value = static_cast<float>(maxBet - bet) / pot;

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

	float BetSize::ToPot(Value value)
	{
		switch (value)
		{
			case VeryLow:	return 0.0f;
			case Low:		return 0.3f;
			case Normal:	return 0.5f;
			case High:		return 0.7f;
			case Huge:		return 1.0f;
			default: return VeryLow;
		}
	}

	float BetSize::ToStack(Value value)
	{
		switch (value)
		{
			case VeryLow:	return 0.0f;
			case Low:		return 0.3f;
			case Normal:	return 0.5f;
			case High:		return 0.7f;
			case Huge:		return 1.0f;
			default: return VeryLow;
		}
	}

};

