#ifndef Danger_h__
#define Danger_h__

#include <string>

namespace pcmn
{
	struct Danger
	{
		enum Value
		{
			Low			= 0,
			Normal		= 1,
			High		= 2,
			Max			= High
		};

		static std::string ToString(Value value);
	};

};

#endif // Danger_h__
