#include "Params.h"

namespace neuro
{

	Params::Params()
		: m_WinRate(pcmn::WinRate::VeryLow)
		, m_Position(pcmn::Player::Position::Early)
		, m_BetPotSize(pcmn::BetSize::VeryLow)
		, m_BetStackSize(pcmn::BetSize::VeryLow)
		, m_ActivePlayers(pcmn::Player::Count::One)
		, m_MostAggressiveStyle(pcmn::Player::Style::Passive)
		, m_UnusualStyle(pcmn::Player::Style::Passive)
		, m_BotStyle(pcmn::Player::Style::Passive)
		, m_BotStackSize(pcmn::StackSize::Small)
		, m_CheckFold(false)
		, m_BetCall(false)
		, m_RaiseReraise(false)
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
			<< m_MostAggressiveStyle << " " 
			<< m_UnusualStyle << " " 
			<< m_BotStyle << " " 
			<< m_BotStackSize << " " 
			<< m_CheckFold << " " 
			<< m_BetCall << " " 
			<< m_RaiseReraise << " " 
			<< std::endl;
	}

	std::vector<int>::const_iterator Params::Deserialize(std::vector<int>::const_iterator it)
	{
		m_WinRate = static_cast<pcmn::WinRate::Value>(*it++);
		m_Position = static_cast<pcmn::Player::Position::Value>(*it++);
		m_BetPotSize = static_cast<pcmn::BetSize::Value>(*it++);
		m_BetStackSize = static_cast<pcmn::BetSize::Value>(*it++);
		m_ActivePlayers = static_cast<pcmn::Player::Count::Value>(*it++);
		m_MostAggressiveStyle = static_cast<pcmn::Player::Style::Value>(*it++);
		m_UnusualStyle = static_cast<pcmn::Player::Style::Value>(*it++);
		m_BotStyle = static_cast<pcmn::Player::Style::Value>(*it++);
		m_BotStackSize = static_cast<pcmn::StackSize::Value>(*it++);

		m_CheckFold = !!*it++;
		m_BetCall = !!*it++;
		m_RaiseReraise = !!*it++;
		return it;
	}

	void Params::ToNeuroFormat(std::vector<float>& in, std::vector<float>& out) const
	{
		in.clear();
		out.clear();

		in.push_back(static_cast<float>(m_WinRate) / pcmn::WinRate::Max);
		in.push_back(static_cast<float>(m_Position) / pcmn::Player::Position::Max);
		in.push_back(static_cast<float>(m_BetPotSize) / pcmn::BetSize::Max);
		in.push_back(static_cast<float>(m_BetStackSize) / pcmn::BetSize::Max);
		in.push_back(static_cast<float>(m_ActivePlayers) / pcmn::Player::Count::Max);
		in.push_back(static_cast<float>(m_MostAggressiveStyle) / pcmn::Player::Style::Max);
		in.push_back(static_cast<float>(m_UnusualStyle) / pcmn::Player::Style::Max);
		in.push_back(static_cast<float>(m_BotStyle) / pcmn::Player::Style::Max);
		in.push_back(static_cast<float>(m_BotStackSize) / pcmn::StackSize::Max);

		out.push_back(static_cast<float>(m_CheckFold));
		out.push_back(static_cast<float>(m_BetCall));
		out.push_back(static_cast<float>(m_RaiseReraise));
	}

}