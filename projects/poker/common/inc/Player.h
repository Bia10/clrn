#ifndef Player_h__
#define Player_h__

#include "Cards.h"

#include <string>
#include <vector>
#include <cassert>

namespace clnt
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
			Agressive	= 2			//!< raise or bet
		};
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

	Player()
		: m_Name()
		, m_Country()
		, m_Stack(0)
		, m_Bet(0)
		, m_WinSize(0)
		, m_Result()
		, m_State(State::Waiting)
	{
		m_Styles.resize(4, Style::Normal);
	}

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

	static Player& ThisPlayer()
	{
		static Player bot;
		return bot;
	}

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
};
}

#endif // Player_h__
