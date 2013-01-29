#ifndef Actions_h__
#define Actions_h__

#include <string>

namespace pcmn
{
	struct Action
	{
		enum Value
		{
			Unknown		= -1,
			Fold		= 0,
			Check		= 1,
			Call		= 2,
			Bet			= 3,
			Raise		= 4,
			ShowCards	= 5,
			SmallBlind	= 6,
			BigBlind	= 7,
			Ante		= 8,
			Win			= 9,
			Loose		= 10,
			MoneyReturn	= 11,
			SecondsLeft	= 12,
			Rank		= 13,
			WinCards	= 14
		};

		static std::string ToString(Value value);
		static bool IsActive(Value value);
	};
}

#endif // Actions_h__
