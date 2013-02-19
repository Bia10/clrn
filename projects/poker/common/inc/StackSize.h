#ifndef StackSize_h__
#define StackSize_h__

#include <string>

namespace pcmn
{
	struct StackSize
	{
		enum Value
		{
			Small	= 0,	// (< 10 BB)
			Normal	= 1,	// (> 10BB and < max player stack * 2 )
			Big		= 2, 	// (> max player stack * 2)
			Max		= Big
		};

		static std::string ToString(Value value);
		static Value FromValue(std::size_t value, std::size_t bigBlind, std::size_t maxStack);
	};

};
#endif // StackSize_h__
