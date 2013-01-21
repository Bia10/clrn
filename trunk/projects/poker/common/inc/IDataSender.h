#ifndef IDataSender_h__
#define IDataSender_h__

#include "packet.pb.h"

#include <string>

namespace pcmn
{

class IDataSender
{
public:
	struct Statistic
	{

	};

	virtual ~IDataSender() {}
	virtual void OnGameFinished(const net::Packet& packet) = 0;
};

}

#endif // IDataSender_h__
