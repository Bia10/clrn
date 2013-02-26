#ifndef Player_h__
#define Player_h__

#include "Cards.h"
#include "IActionsQueue.h"
#include "TableContext.h"

#include <string>
#include <vector>
#include <cassert>

namespace pcmn
{
class Player
{
public:

	//! Player game styles
	struct Style
	{
		enum Value
		{
			Passive		= 0,		//!< check or fold
			Normal		= 1,		//!< call
			Aggressive	= 2,		//!< raise/bet/reraise
			Max			= Aggressive
		};

		static std::string ToString(Value value);
	};

	//! Result of the player game
	struct Result
	{
		enum Value
		{
			Unknown			= 0,
			WinByCards		= 1,
			WinByLuck		= 2,
			WinByRaise		= 3,
			LooseWithCards	= 4,
			LooseWithFold	= 5,
			LooseWithNoLuck	= 6
		};

	};

	//! Player state
	struct State
	{
		enum Value
		{
			Waiting	= 0,
			Fold	= 1,
			InPot	= 2,
			AllIn	= 3,
			AFK		= 4
		};
	};
	
	//! Player positions
	struct Position
	{
		enum Value
		{
			Early	= 0,
			Middle	= 1,
			Later	= 2,
			Max = Later
		};
		static std::string ToString(Value value);
	};

	//! Players count
	struct Count
	{
		enum Value
		{
			One			= 0,
			Two			= 1,
			ThreeOrMOre	= 2,
			Max			= ThreeOrMOre
		};
		static std::string ToString(Value value);
	};

	Player()
		: m_Name()
		, m_Country()
		, m_Stack(0)
		, m_Bet(0)
		, m_WinSize(0)
		, m_Result()
		, m_State(State::Waiting)
		, m_Next(0)
		, m_Previous(0)
	{
		m_Styles.resize(4, Style::Normal);
	}

	Player(const std::string& name, const std::size_t stack, Player* next = 0, Player* prev = 0)
		: m_Name(name)
		, m_Country()
		, m_Stack(stack)
		, m_Bet(0)
		, m_WinSize(0)
		, m_Result()
		, m_State(State::Waiting)
		, m_Next(next)
		, m_Previous(prev)
	{
		m_Styles.resize(4, Style::Normal);
	}
	~Player();

	typedef std::vector<Player> List;
	typedef std::vector<Style::Value> Styles;

	std::string Name() const				{ return m_Name; }
	void Name(const std::string& val)		{ m_Name = val; }
	std::string Country() const				{ return m_Country; }
	void Country(const std::string& val)	{ m_Country = val; }
	std::size_t Stack() const				{ return m_Stack; }
	void Stack(std::size_t val)				{ m_Stack = val; }
	std::size_t Bet() const					{ return m_Bet; }
	void Bet(std::size_t val)				{ m_Bet = val; }
	Result::Value Result() const			{ return m_Result; }
	void Result(Result::Value val)			{ m_Result = val; }
	const Card::List& Cards() const			{ return m_Cards; }
	void Cards(const Card::List& val)		{ assert(val.size() == 2); m_Cards = val; }
	std::size_t WinSize() const				{ return m_WinSize; }
	void WinSize(std::size_t val)			{ m_WinSize = val; }
	State::Value State() const				{ return m_State; }
	void State(State::Value val)			{ m_State = val; }

	void SetStyle(std::size_t phase, Style::Value style)
	{
		if (style > m_Styles[phase])
			m_Styles[phase] = style;
	}

	Style::Value GetStyle(std::size_t phase)
	{
		return m_Styles[phase];
	}

	static Player& ThisPlayer()
	{
		static Player bot;
		return bot;
	}

	IActionsQueue::Event::Value Do(IActionsQueue& actions, TableContext& table);
	Player* GetNext() const { return m_Next; }

	bool operator == (const Player& other) const;

private:
	std::string m_Name;		
	std::string m_Country;	
	std::size_t m_Stack;
	std::size_t m_Bet;		//!< player bet on this street
	std::size_t m_WinSize;	//!< player possible win size
	Styles m_Styles;		//!< player styles during this game
	Result::Value m_Result;	//!< player game result
	Card::List m_Cards;		//!< player cards
	State::Value m_State;	//!< player state
	Player* m_Next;			//!< next player on the table
	Player* m_Previous;		//!< previous player on the table
};
}

#endif // Player_h__