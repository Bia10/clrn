#include "Params.h"

#include <boost/format.hpp>

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

	bool Params::operator < (const Params& other) const
	{
		if (m_WinRate != other.m_WinRate)
			return m_WinRate < other.m_WinRate;
		if (m_Position != other.m_Position)
			return m_Position < other.m_Position;
		if (m_BetPotSize != other.m_BetPotSize)
			return m_BetPotSize < other.m_BetPotSize;
		if (m_BetStackSize != other.m_BetStackSize)
			return m_BetStackSize < other.m_BetStackSize;
		if (m_ActivePlayers != other.m_ActivePlayers)
			return m_ActivePlayers < other.m_ActivePlayers;
		if (m_Danger != other.m_Danger)
			return m_Danger < other.m_Danger;
		if (m_BotAverageStyle != other.m_BotAverageStyle)
			return m_BotAverageStyle < other.m_BotAverageStyle;
		if (m_BotStyle != other.m_BotStyle)
			return m_BotStyle < other.m_BotStyle;
		if (m_BotStackSize != other.m_BotStackSize)
			return m_BotStackSize < other.m_BotStackSize;
		return false;
	}

	std::string Params::ToString() const
	{
		return (boost::format("win: [%s], pos: [%s], pot: [%s], stack: [%s], players: [%s], danger: [%s], bot avg style: [%s], bot style: [%s], bot stack: [%s], decision: [%s]")
			% pcmn::WinRate::ToString(m_WinRate)
			% pcmn::Player::Position::ToString(m_Position)
			% pcmn::BetSize::ToString(m_BetPotSize)
			% pcmn::BetSize::ToString(m_BetStackSize)
			% pcmn::Player::Count::ToString(m_ActivePlayers)
			% pcmn::Danger::ToString(m_Danger)
			% pcmn::Player::Style::ToString(m_BotAverageStyle)
			% pcmn::Player::Style::ToString(m_BotStyle)
			% pcmn::StackSize::ToString(m_BotStackSize)
			% (m_CheckFold ? "Check/Fold" : (m_CheckCall ? "Check/Call" : "Bet/Raise"))
		).str();
	}

	bool Params::IsDecisionEquals(const Params& other) const
	{
		return 
			m_CheckFold == other.m_CheckFold
			&&
			m_CheckCall == other.m_CheckCall
			&&
			m_BetRaise == other.m_BetRaise;
	}

	bool Params::operator == (const Params& other) const
	{
		return !memcmp(this, &other, sizeof(Params));
	}

}