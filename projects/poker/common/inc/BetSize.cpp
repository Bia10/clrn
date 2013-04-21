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

	BetSize::Value BetSize::FromParams(unsigned bet, unsigned maxBet, unsigned pot, unsigned stack, unsigned bigBlind)
	{
        if (!bet)
            return BetSize::VeryLow; 

        if (bet <= bigBlind)
            return BetSize::Low;

        const float value = bet > maxBet ? static_cast<float>(bet) / pot : static_cast<float>(maxBet - bet) / pot;
        if (bet <= bigBlind * 3 && value < 0.6f)
            return BetSize::Normal;
        if (value < 1.5f)
            return BetSize::High;

        return BetSize::Huge;
	}

};

