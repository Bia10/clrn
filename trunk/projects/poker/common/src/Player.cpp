#include "Player.h"

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
			CASE(ThreeOrMOre)
		default: return "unknown";
		}
#undef CASE
	}


	IActionsQueue::Event::Value Player::Do(IActionsQueue& actions, TableContext& table)
	{
		Action::Value action = Action::Unknown;
		unsigned amount = 0;

		if (!actions.Extract(action, amount))
			return IActionsQueue::Event::NeedDecition;

		switch (action)
		{
		case pcmn::Action::Fold: 
			m_State = State::Fold;
			return IActionsQueue::Event::Fold;
		case pcmn::Action::Bet: 
		case pcmn::Action::Raise:
			{
				const unsigned difference = m_Bet - amount;
				m_Stack -= difference;
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
		if (m_Previous && m_Next)
		{
			m_Previous->m_Next = m_Next;
			m_Next->m_Previous = m_Previous;
		}
		else
		if (m_Previous)
		{
			m_Previous->m_Next = 0;
		}
		else
		if (m_Next)
		{
			m_Next->m_Previous = 0;
		}

	}

}
