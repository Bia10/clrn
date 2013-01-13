#ifndef Actions_h__
#define Actions_h__

#include <string>

namespace clnt
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
			Show		= 5,
			SmallBlind	= 6,
			BigBlind	= 7,
			Ante		= 8,
			Win			= 9,
			Loose		= 10
		};

		static std::string ToString(Value value);
	};

}

#endif // Actions_h__
