#include "Actions.h"

namespace clnt
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
				CASE(Show)
				CASE(SmallBlind)
				CASE(BigBlind)
				CASE(Ante)
				CASE(Win)
				CASE(Loose)
		default: return "unknown";
		}
#undef CASE
	}

}
