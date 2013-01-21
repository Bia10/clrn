#include "Actions.h"

namespace pcmn
{
	std::string Action::ToString(const Action::Value value)
	{
#define CASE(x) case x: return #x;
		switch (value)
		{
			CASE(Fold)
			CASE(Check)
			CASE(Call)
			CASE(Bet)
			CASE(Raise)
			CASE(ShowCards)
			CASE(SmallBlind)
			CASE(BigBlind)
			CASE(Ante)
			CASE(Win)
			CASE(Loose)
			CASE(MoneyReturn)
			CASE(SecondsLeft)
			CASE(Rank)
			CASE(WinCards)
		default: return "unknown";
		}
#undef CASE
	}
}
