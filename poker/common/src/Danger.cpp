#include "Danger.h"

namespace pcmn
{


	std::string Danger::ToString(Value value)
	{
#define CASE(x) case x: return #x;
		switch (value)
		{
			CASE(Low)
			CASE(Normal)
			CASE(High)
		default: return "unknown";
		}
#undef CASE
	}

}