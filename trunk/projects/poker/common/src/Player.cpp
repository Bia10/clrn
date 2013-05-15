#include "Player.h"
#include "TableContext.h"
#include "Exception.h"

//#include <mongo/bson/bson.h>

namespace pcmn
{
	std::string Player::Style::ToString(Value value)
	{
#define CASE(x) case x: return #x;
		switch (value)
		{
			CASE(Passive)
			CASE(Normal)
			CASE(Aggressive)
		default: return "unknown";
		}
#undef CASE
	}

	std::string Player::Position::ToString(Value value)
	{
#define CASE(x) case x: return #x;
		switch (value)
		{
			CASE(Early)
			CASE(Middle)
			CASE(Later)
		default: return "unknown";
		}
#undef CASE
	}


	std::string Player::Count::ToString(Value value)
	{
#define CASE(x) case x: return #x;
		switch (value)
		{
			CASE(One)
			CASE(Two)
			CASE(ThreeOrMore)
		default: return "unknown";
		}
#undef CASE
	}

	Player::Count::Value Player::Count::FromValue(unsigned value)
	{
		if (value == 1)
			return One;
		else
		if (value == 2)
			return Two;
		else
			return ThreeOrMore;
	}

	bool Player::operator==(const Player& other) const
	{
		return m_Name == other.m_Name;
	}

	Player::~Player()
	{
	}

	void Player::PushAction(unsigned street, Action::Value action, BetSize::Value value, Position::Value pos)
	{
        if (!Action::IsActive(action))
			return; // don't collect useless for statistics actions

        if (m_Actions.size() <= street)
            m_Actions.resize(street + 1);

		m_Actions[street].push_back(ActionDesc());
        ActionDesc& actionDesc = m_Actions[street].back();
        actionDesc.m_Id = action;
        actionDesc.m_Amount = value;
        actionDesc.m_Position = pos;
	}

    void Player::Reset()
    {
        m_TotalBet = 0;
        m_Bet = 0;
        m_Cards.clear();
        m_State = State::Waiting;
        m_Actions.clear();
        m_Styles.clear();
    }


    bool Player::ActionDesc::operator==(const ActionDesc& other) const
    {
        return m_Id == other.m_Id && m_Amount == other.m_Amount;
    }

    std::ostream& operator<<(std::ostream& s, const Player& p)
    {
        s << p.Name() << ":" << p.Stack();
        return s;
    }

}
