#ifndef PacketActionsQueue_h__
#define PacketActionsQueue_h__

#include "packet.pb.h"
#include "IActionsQueue.h"

namespace pcmn
{

class PacketActions : public IActionsQueue
{
public:

	PacketActions(const net::Packet& packet);

	virtual bool Extract(Action::Value& action, unsigned& amount) const override;

private:

	mutable int m_Index;
	mutable int m_Street;
	const net::Packet& m_Packet;
};

}


#endif // PacketActionsQueue_h__
