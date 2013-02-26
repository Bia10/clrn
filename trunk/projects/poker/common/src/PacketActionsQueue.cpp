#include "PacketActionsQueue.h"

namespace pcmn
{

PacketActions::PacketActions(const net::Packet::Phase& street) : m_Index(0), m_Street(street)
{

}

bool PacketActions::Extract(Action::Value& action, unsigned& amount) const 
{
	if (m_Index >= m_Street.actions_size())
		return false;

	action = static_cast<Action::Value>(m_Street.actions(m_Index).id());
	amount = static_cast<unsigned>(m_Street.actions(m_Index).amount());
	++m_Index;
	return true;
}

}