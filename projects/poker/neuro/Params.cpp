#include "Params.h"

namespace neuro
{

	Params::Params()
		: m_WinRate(pcmn::WinRate::VeryLow)
		, m_Position(pcmn::Player::Position::Early)
		, m_BetPotSize(pcmn::BetSize::VeryLow)
		, m_BetStackSize(pcmn::BetSize::VeryLow)
		, m_ActivePlayers(pcmn::Player::Count::One)
		, m_Danger(pcmn::Danger::Low)
		, m_BotAverageStyle(pcmn::Player::Style::Passive)
		, m_BotStyle(pcmn::Player::Style::Passive)
		, m_BotStackSize(pcmn::StackSize::Small)
		, m_CheckFold(false)
		, m_CheckCall(false)
		, m_BetRaise(false)
	{

	}

	void Params::Serialize(std::ostream& stream) const
	{
		stream 
			<< m_WinRate << " " 
			<< m_Position << " " 
			<< m_BetPotSize << " " 
			<< m_BetStackSize << " " 
			<< m_ActivePlayers << " " 
			<< m_Danger << " " 
			<< m_BotAverageStyle << " " 
			<< m_BotStyle << " " 
			<< m_BotStackSize << " " 
			<< m_CheckFold << " " 
			<< m_CheckCall << " " 
			<< m_BetRaise << " " 
			<< std::endl;
	}

	std::vector<int>::const_iterator Params::Deserialize(std::vector<int>::const_iterator it)
	{
		m_WinRate = static_cast<pcmn::WinRate::Value>(*it++);
		m_Position = static_cast<pcmn::Player::Position::Value>(*it++);
		m_BetPotSize = static_cast<pcmn::BetSize::Value>(*it++);
		m_BetStackSize = static_cast<pcmn::BetSize::Value>(*it++);
		m_ActivePlayers = static_cast<pcmn::Player::Count::Value>(*it++);
		m_Danger = static_cast<pcmn::Danger::Value>(*it++);
		m_BotAverageStyle = static_cast<pcmn::Player::Style::Value>(*it++);
		m_BotStyle = static_cast<pcmn::Player::Style::Value>(*it++);
		m_BotStackSize = static_cast<pcmn::StackSize::Value>(*it++);

		m_CheckFold = !!*it++;
		m_CheckCall = !!*it++;
		m_BetRaise = !!*it++;
		return it;
	}

	void Params::ToNeuroFormat(std::vector<float>& in, std::vector<float>& out) const
	{
		in.clear();
		out.clear();

		in.push_back(pcmn::WinRate::ToValue(m_WinRate));
		in.push_back(static_cast<float>(m_Position) / pcmn::Player::Position::Max);
		in.push_back(pcmn::BetSize::ToPot(m_BetPotSize));
		in.push_back(pcmn::BetSize::ToStack(m_BetStackSize));
		in.push_back(static_cast<float>(m_ActivePlayers) / pcmn::Player::Count::Max);
		in.push_back(static_cast<float>(m_Danger) / pcmn::Danger::Max);
		in.push_back(static_cast<float>(m_BotAverageStyle) / pcmn::Player::Style::Max);
		in.push_back(static_cast<float>(m_BotStyle) / pcmn::Player::Style::Max);
		in.push_back(static_cast<float>(m_BotStackSize) / pcmn::StackSize::Max);

		out.push_back(static_cast<float>(m_CheckFold));
		out.push_back(static_cast<float>(m_CheckCall));
		out.push_back(static_cast<float>(m_BetRaise));
	}

}