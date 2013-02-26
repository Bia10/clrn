#ifndef PacketActionsQueue_h__
#define PacketActionsQueue_h__

#include "packet.pb.h"
#include "IActionsQueue.h"

namespace pcmn
{

class PacketActions : public IActionsQueue
{
public:

	PacketActions(const net::Packet::Phase& street);

	virtual bool Extract(Action::Value& action, unsigned& amount) const override;

private:

	mutable int m_Index;
	const net::Packet::Phase& m_Street;
};

}


#endif // PacketActionsQueue_h__
