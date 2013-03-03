#include "PacketActionsQueue.h"

namespace pcmn
{

PacketActions::PacketActions(const net::Packet& packet) 
	: m_Index(0)
	, m_Packet(packet)
	, m_Street(0)
{

}

bool PacketActions::Extract(Action::Value& action, unsigned& amount, unsigned& player) const 
{
	if (m_Street >= m_Packet.phases_size())
		return false;

	if (m_Index >= m_Packet.phases(m_Street).actions_size())
	{
		m_Index = 0;
		++m_Street;
	}

	if (m_Street >= m_Packet.phases_size())
		return false;

	action = static_cast<Action::Value>(m_Packet.phases(m_Street).actions(m_Index).id());
	amount = static_cast<unsigned>(m_Packet.phases(m_Street).actions(m_Index).amount());
	player = static_cast<unsigned>(m_Packet.phases(m_Street).actions(m_Index).player());
	++m_Index;

	if (action > Action::BigBlind)
		return Extract(action, amount, player);

	return true;
}

}