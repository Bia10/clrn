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

	BetSize::Value BetSize::FromAction(unsigned bet, unsigned pot, unsigned stack, unsigned bigBlind)
	{
        if (!bet)
            return BetSize::VeryLow; 

        if (bet <= bigBlind)
            return BetSize::Low;

        const float value = static_cast<float>(bet) / pot;
        if (bet <= bigBlind * 3 && value < 0.6f)
            return BetSize::Normal;
        if (value < 1.5f)
            return BetSize::High;

        return BetSize::Huge;
	}

    BetSize::Value BetSize::FromDecision(unsigned bet, unsigned maxBet, unsigned pot, unsigned stack, unsigned bigBlind)
    {
        if (pot > maxBet)
            pot -= maxBet; // minus max bet

        if (!maxBet)
            return BetSize::VeryLow; 

        if (bet >= maxBet * 2 / 3)
            return BetSize::VeryLow; // bet already more than 2/3

        if (bet >= maxBet / 2 && maxBet <= bigBlind * 3)
            return BetSize::VeryLow; // bet at least half in small bet

        if (maxBet - bet < pot / 5)
            return BetSize::VeryLow; // pot is so big

        if (maxBet - bet <= bigBlind)
            return BetSize::Low;

        const bool isBigBetToStack = maxBet > 0.8f * stack;
        const float value = static_cast<float>(maxBet - bet) / pot;
        if (bet <= bigBlind * 3 && value < 0.6f && !isBigBetToStack)
            return BetSize::Normal;
        if (value < 1.5f && !isBigBetToStack)
            return BetSize::High;

        return BetSize::Huge;   
    }

};

