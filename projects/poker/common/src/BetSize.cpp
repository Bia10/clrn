#include "BetSize.h"

#include <assert.h>

namespace pcmn
{
	std::string BetSize::ToString(Value value)
	{
#define CASE(x) case x: return #x;
		switch (value)
		{
			CASE(NoBet)
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
            return BetSize::NoBet; 

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
        if (!maxBet)
            return BetSize::NoBet; 

        if (bet >= maxBet * 2 / 3)
            return BetSize::NoBet; // bet already more than 2/3

        if (bet >= maxBet / 2 && maxBet <= bigBlind * 3)
            return BetSize::NoBet; // bet at least half in small bet

        if (maxBet - bet < pot / 5)
            return BetSize::NoBet; // pot is so big

        if (maxBet - bet <= bigBlind)
            return BetSize::Low;

        const bool isBigBetToStack = maxBet > 0.8f * stack;
        const float value = static_cast<float>(maxBet - bet) / pot;
        if (bet <= bigBlind * 3 && value < 0.6f && !isBigBetToStack && maxBet - bet < bigBlind * 6)
            return BetSize::Normal;
        if (value < 1.5f && !isBigBetToStack)
            return BetSize::High;

        return BetSize::Huge;   
    }

    unsigned BetSize::FromValue(Value v, unsigned pot, unsigned stack, unsigned alreadyBet, unsigned bigBlind)
    {
        switch (v)
        {
        case NoBet:     return 0;
        case Low:       return bigBlind;
        case Normal:    return bigBlind * 3;
        case High:      return pot;
        case  Huge:     return stack;
        default: assert(false); return 0;
        }
    }

};

