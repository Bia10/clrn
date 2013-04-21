#include "Player.h"
#include "TableContext.h"
#include "Exception.h"

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

	void Player::PushAction(unsigned street, Action::Value action, BetSize::Value value)
	{
        if (!Action::IsUseful(action))
			return; // don't collect useless for statistics actions

		m_Actions.push_back(ActionDesc());
		ActionDesc& actionDesc = m_Actions.back();
		actionDesc.m_Action = action;
		actionDesc.m_Value = value;
		actionDesc.m_Street = street;
	}

}
