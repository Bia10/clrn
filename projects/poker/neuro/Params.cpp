#include "Params.h"
#include "Recordset.h"
#include "IDatabase.h"
#include "Exception.h"

#include <boost/format.hpp>

namespace neuro
{

	Params::Params()
		: m_WinRate(pcmn::WinRate::VeryLow)
		, m_Position(pcmn::Player::Position::Early)
		, m_BetSize(pcmn::BetSize::NoBet)
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
			<< m_BetSize << " " 
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
		m_BetSize = static_cast<pcmn::BetSize::Value>(*it++);
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
		in.push_back(static_cast<float>(m_BetSize) / pcmn::BetSize::Max);
		in.push_back(static_cast<float>(m_ActivePlayers) / pcmn::Player::Count::Max);
		in.push_back(static_cast<float>(m_Danger) / pcmn::Danger::Max);
		in.push_back(static_cast<float>(m_BotAverageStyle) / pcmn::Player::Style::Max);
		in.push_back(static_cast<float>(m_BotStyle) / pcmn::Player::Style::Max);
		in.push_back(static_cast<float>(m_BotStackSize) / pcmn::StackSize::Max);

		out.push_back(static_cast<float>(m_CheckFold));
		out.push_back(static_cast<float>(m_CheckCall));
		out.push_back(static_cast<float>(m_BetRaise));
	}

    void Params::FromNeuroFormat(const std::vector<float>& in, const std::vector<float>& out)
    {
        int counter = 0;
        m_WinRate = pcmn::WinRate::FromValue(in[counter++] * 100);
        m_Position = static_cast<pcmn::Player::Position::Value>(int(in[counter++] * pcmn::Player::Position::Max));
        m_BetSize = static_cast<pcmn::BetSize::Value>(int(in[counter++] * pcmn::BetSize::Max));
        m_ActivePlayers = static_cast<pcmn::Player::Count::Value>(int(in[counter++] * pcmn::Player::Count::Max));
        m_Danger = static_cast<pcmn::Danger::Value>(int(in[counter++] * pcmn::Danger::Max));
        m_BotAverageStyle = static_cast<pcmn::Player::Style::Value>(int(in[counter++] * pcmn::Player::Style::Max));
        m_BotStyle = static_cast<pcmn::Player::Style::Value>(int(in[counter++] * pcmn::Player::Style::Max));
        m_BotStackSize = static_cast<pcmn::StackSize::Value>(int(in[counter++] * pcmn::StackSize::Max));

        m_CheckFold = false;
        m_CheckCall = false;
        m_BetRaise = false;

        if (out.empty())
            return;

        if (out[0] > out[1] && out[0] > out[2])
			m_CheckFold = true;
		else
		if (out[1] > out[0] && out[1] > out[2])
			m_CheckCall = true;
		else
		if (out[2] > out[0] && out[2] > out[1])
			m_BetRaise = true;
    }

	bool Params::operator < (const Params& other) const
	{
		if (m_WinRate != other.m_WinRate)
			return m_WinRate < other.m_WinRate;
		if (m_Position != other.m_Position)
			return m_Position < other.m_Position;
		if (m_BetSize != other.m_BetSize)
			return m_BetSize < other.m_BetSize;
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
		return (boost::format("win: [%s], pos: [%s], bet: [%s], players: [%s], danger: [%s], bot avg style: [%s], bot style: [%s], bot stack: [%s], decision: [%s]")
			% pcmn::WinRate::ToString(m_WinRate)
			% pcmn::Player::Position::ToString(m_Position)
			% pcmn::BetSize::ToString(m_BetSize)
			% pcmn::Player::Count::ToString(m_ActivePlayers)
			% pcmn::Danger::ToString(m_Danger)
			% pcmn::Player::Style::ToString(m_BotAverageStyle)
			% pcmn::Player::Style::ToString(m_BotStyle)
			% pcmn::StackSize::ToString(m_BotStackSize)
			% (m_CheckFold ? "Check/Fold" : (m_CheckCall ? "Check/Call" : m_BetRaise ? "Bet/Raise" : "Not set"))
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

    unsigned Params::Hash() const
    {
        unsigned result = 0;
        unsigned rate = 1;

        result += rate * m_BotStackSize;
        rate *= (pcmn::StackSize::Max + 1);

        result += rate * m_BotStyle;
        rate *= (pcmn::Player::Style::Max + 1);

        result += rate * m_BotAverageStyle;
        rate *= (pcmn::Player::Style::Max + 1);

        result += rate * m_Danger;
        rate *= (pcmn::Danger::Max + 1);

        result += rate * m_ActivePlayers;
        rate *= (pcmn::Player::Count::Max + 1);

        result += rate * m_BetSize;
        rate *= (pcmn::BetSize::Max + 1);

        result += rate * m_Position;
        rate *= (pcmn::Player::Position::Max + 1);

        result += rate * m_WinRate;
        rate *= (pcmn::WinRate::Max + 1);

        return result;
    }

    unsigned Params::MaxHash()
    {
        unsigned rate = 1;
        rate *= (pcmn::StackSize::Max + 1);
        rate *= (pcmn::Player::Style::Max + 1);
        rate *= (pcmn::Player::Style::Max + 1);
        rate *= (pcmn::Danger::Max + 1);
        rate *= (pcmn::Player::Count::Max + 1);
        rate *= (pcmn::BetSize::Max + 1);
        rate *= (pcmn::Player::Position::Max + 1);
        rate *= (pcmn::WinRate::Max + 1);
        return rate;
    }

    void Params::SetParams(unsigned hash)
    {
        unsigned rate = 1;

        rate = (pcmn::StackSize::Max + 1);
        m_BotStackSize = static_cast<pcmn::StackSize::Value>(hash % rate);
        hash -= m_BotStackSize;
        hash /= rate;

        rate = (pcmn::Player::Style::Max + 1);
        m_BotStyle = static_cast<pcmn::Player::Style::Value>(hash % rate);
        hash -= m_BotStyle;
        hash /= rate;

        rate = (pcmn::Player::Style::Max + 1);
        m_BotAverageStyle = static_cast<pcmn::Player::Style::Value>(hash % rate);
        hash -= m_BotAverageStyle;
        hash /= rate;

        rate = (pcmn::Danger::Max + 1);
        m_Danger = static_cast<pcmn::Danger::Value>(hash % rate);
        hash -= m_Danger;
        hash /= rate;

        rate = (pcmn::Player::Count::Max + 1);
        m_ActivePlayers = static_cast<pcmn::Player::Count::Value>(hash % rate);
        hash -= m_ActivePlayers;
        hash /= rate;

        rate = (pcmn::BetSize::Max + 1);
        m_BetSize = static_cast<pcmn::BetSize::Value>(hash % rate);
        hash -= m_BetSize;
        hash /= rate;

        rate = (pcmn::Player::Position::Max + 1);
        m_Position = static_cast<pcmn::Player::Position::Value>(hash % rate);
        hash -= m_Position;
        hash /= rate;

        rate = (pcmn::WinRate::Max + 1);
        m_WinRate = static_cast<pcmn::WinRate::Value>(hash % rate);
        hash -= m_WinRate;
        hash /= rate;
    }

    void Params::Write(sql::IStatement& stmnt) const
    {
        int decision = 0;
        if (m_CheckFold)
            decision = 0;
        else
        if (m_CheckCall)
            decision = 1;
        else
        if (m_BetRaise)
            decision = 2;
        else
            CHECK(false, "Empty parameter", Hash());

        stmnt 
            << m_WinRate
            << m_Position
            << m_BetSize
            << m_ActivePlayers
            << m_Danger
            << m_BotAverageStyle
            << m_BotStyle
            << m_BotStackSize
            << decision;
        CHECK(stmnt.Execute() == 1, "Failed to insert");
    }

    void Params::Read(sql::Recordset& recordset)
    {
        m_BotStackSize = static_cast<pcmn::StackSize::Value>(recordset.Get<int>("bot_stack"));
        m_BotStyle = static_cast<pcmn::Player::Style::Value>(recordset.Get<int>("bot_style"));
        m_BotAverageStyle = static_cast<pcmn::Player::Style::Value>(recordset.Get<int>("bot_avg_style"));
        m_Danger = static_cast<pcmn::Danger::Value>(recordset.Get<int>("danger"));
        m_ActivePlayers = static_cast<pcmn::Player::Count::Value>(recordset.Get<int>("players"));
        m_BetSize = static_cast<pcmn::BetSize::Value>(recordset.Get<int>("bet"));
        m_Position = static_cast<pcmn::Player::Position::Value>(recordset.Get<int>("position"));
        m_WinRate = static_cast<pcmn::WinRate::Value>(recordset.Get<int>("win"));

        const int decision = recordset.Get<int>("decision");
        m_CheckFold = false;
        m_CheckCall = false;
        m_BetRaise = false;

        switch (decision)
        {
        case 0: m_CheckFold = true; break;
        case 1: m_CheckCall = true; break;
        case 2: m_BetRaise = true; break;
        default: assert(false); break;
        }
    }

    std::size_t Params::ReadAll(List& params, sql::IDatabase& db, const std::string& where)
    {
        const bool empty = params.empty();
        if (!empty)
            CHECK(params.size() == MaxHash());

        const sql::Recordset::Ptr statement = db.Fetch(
            std::string(
            "SELECT "
                "win, "
                "position, "
                "bet, "
                "players, "
                "danger, "
                "bot_avg_style, "
                "bot_style, "
                "bot_stack, "
                "decision "
                "FROM decisions ") + (where.empty() ? "" : (std::string("where ") + where))
        );

        std::size_t result = 0;
        while (!statement->Eof())
        {
            Params temp;
            temp.Read(*statement);
            if (empty)
                params.push_back(temp);
            else
                params.at(temp.Hash()) = temp;
            ++(*statement);
            ++result;
        }
        return result;
    }

    void Params::WriteAll(const List& params, sql::IDatabase& db)
    {
        const sql::IStatement::Ptr statement = db.CreateStatement
        (
            "INSERT INTO decisions "
                        "(win, "
                         "position, "
                         "bet, "
                         "players, "
                         "danger, "
                         "bot_avg_style, "
                         "bot_style, "
                         "bot_stack, "
                         "decision) "
            "VALUES      ( ?, "
                          "?, "
                          "?, "
                          "?, "
                          "?, "
                          "?, "
                          "?, "
                          "?, "
                          "?) "
         );

        for (const Params& p : params)
            p.Write(*statement);
    }

}