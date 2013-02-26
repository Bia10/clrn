#ifndef IActionsQueue_h__
#define IActionsQueue_h__

#include "Actions.h"

#include <string>

namespace pcmn
{

//! Actions sequence
class IActionsQueue
{
public:
	struct Event
	{
		enum Value
		{
			Fold			= 0,
			Call			= 1,
			Raise			= 2,
			NeedDecition	= 3
		};
	};

public:
	virtual ~IActionsQueue() {}

	virtual bool Extract(Action::Value& action, unsigned& amount) const = 0;
};

}


#endif // IActionsQueue_h__
