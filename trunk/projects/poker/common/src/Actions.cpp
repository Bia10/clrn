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

	bool Action::IsActive(Value value)
	{
		switch (value)
		{
			case Check:
			case Call:
			case Bet:
			case Raise:
			case SmallBlind:
			case BigBlind:
				return true;
		}
		return false;
	}

	Action::Value Action::FromString(const std::string& text)
	{
#define CASE(x) if (text == #x) return x;
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
		}
#undef CASE
		return Unknown;
	}

}
