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

	IActionsQueue::Event::Value Player::Do(IActionsQueue& actions, TableContext& table)
	{
		Action::Value action = Action::Unknown;
		unsigned amount = 0;
		unsigned player = 0;

		if (!actions.Extract(action, amount, player))
			return IActionsQueue::Event::NeedDecision;

		if (player != m_Index)
			throw pcmn::Player::BadIndex(m_Index, player);

		if (!table.m_BigBlind)
		{
			if (action == Action::Value::SmallBlind)
				table.m_BigBlind = amount * 2;
			else
			if (action == Action::Value::BigBlind)
				table.m_BigBlind = amount;
		}


		table.m_LastAction = action;
		table.m_LastAmount = amount;

		switch (action)
		{
		case pcmn::Action::Fold: 
			m_State = State::Fold;
			return IActionsQueue::Event::Fold;
		case pcmn::Action::Bet: 
		case pcmn::Action::Raise:
			{
				const unsigned difference = amount - m_Bet;
				m_Stack -= difference;
				if (static_cast<int>(m_Stack) < 0)
					m_Stack = 0;

				m_State = m_Stack ? State::Waiting : State::AllIn;
				table.m_Pot += difference;
				m_Bet += difference;
				if (m_Bet > table.m_MaxBet)
					table.m_MaxBet = m_Bet;
				return IActionsQueue::Event::Raise;
			}
		case pcmn::Action::Call: 
		case pcmn::Action::SmallBlind: 
		case pcmn::Action::BigBlind:
			m_Stack -= amount;
			if (static_cast<int>(m_Stack) < 0)
				m_Stack = 0;

			if (m_Stack)
				m_State = action == pcmn::Action::Call ? State::Waiting : State::InPot;
			else
				m_State = State::AllIn;	

			table.m_Pot += amount;
			m_Bet += amount;
			if (m_Bet > table.m_MaxBet)
				table.m_MaxBet = m_Bet;
			break;
		
		}

		return IActionsQueue::Event::Call;
	}

	bool Player::operator==(const Player& other) const
	{
		return m_Name == other.m_Name;
	}

	Player::~Player()
	{
	}

	void Player::DeleteLinks()
	{
		assert(m_Previous.lock() && m_Next.lock());

		m_Previous.lock()->m_Next = m_Next;
		m_Next.lock()->m_Previous = m_Previous;
        m_Previous.reset();
        m_Next.reset();
	}

	void Player::PushAction(unsigned street, Action::Value value, float potAmount)
	{
		if (value > pcmn::Action::Raise)
			return; // don't collect useless for statistics actions

		m_Actions.push_back(ActionDesc());
		ActionDesc& action = m_Actions.back();
		action.m_Action = value;
		action.m_PotAmount = potAmount;
		action.m_Street = street;
	}

}
