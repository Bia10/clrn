#ifndef Player_h__
#define Player_h__

#include "Cards.h"
#include "Actions.h"
#include "BetSize.h"

#include <string>
#include <vector>
#include <cassert>
#include <exception>
#include <deque>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace pcmn
{
struct TableContext;

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

	//! Player state
	struct State
	{
		enum Value
		{
			Called	= 0,    // player called all bets
			Folded	= 1,    // player folded
			Waiting	= 2     // player waiting for his turn
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
			ThreeOrMore	= 2,
			Max			= ThreeOrMore
		};
		static std::string ToString(Value value);
		static Value FromValue(unsigned value);
	};

	//! Bad index exception
	class BadIndex : public std::exception
	{
	public:
		BadIndex(unsigned expected, unsigned got) : m_Expected(expected), m_Got(got) {}
		const unsigned Got() const { return m_Got; }
		const unsigned Expected() const { return m_Expected; }
	private:
		const unsigned m_Expected;
		const unsigned m_Got;
	};

	//! Player action descriptor
	struct ActionDesc
	{
        typedef std::vector<ActionDesc> List;
		Action::Value m_Id;
		BetSize::Value m_Amount;
        Position::Value m_Position;
	};

	typedef std::vector<Player> List;
	typedef std::vector<Style::Value> Styles;
	typedef std::vector<ActionDesc::List> Actions;
    typedef std::deque<Player> Queue;

	Player()
		: m_Name()
		, m_Country()
		, m_Stack(0)
		, m_Bet(0)
		, m_State(State::Called)
        , m_TotalBet(0)
        , m_Afk(false)
	{
		m_Styles.resize(4, Style::Normal);
	}

	Player(const std::string& name, const std::size_t stack)
		: m_Name(name)
		, m_Country()
		, m_Stack(stack)
		, m_Bet(0)
		, m_State(State::Called)
		, m_TotalBet(0)
        , m_Afk(false)
	{
		m_Styles.resize(4, Style::Normal);
	}
	~Player();

	const std::string& Name() const			{ assert(!m_Name.empty()); return m_Name; }
	void Name(const std::string& val)		{ assert(!val.empty()); m_Name = val; }
	const std::string& Country() const		{ return m_Country; }
	void Country(const std::string& val)	{ m_Country = val; }
	std::size_t Stack() const				{ return m_Stack; }
	void Stack(std::size_t val)				{ m_Stack = val; }
	std::size_t Bet() const					{ return m_Bet; }
	void Bet(std::size_t val)				{ m_Bet = val; }
	const Card::List& Cards() const			{ return m_Cards; }
	void Cards(const Card::List& val)		{ assert(val.size() == 2); m_Cards = val; }
	State::Value State() const				{ return m_State; }
	void State(State::Value val)			{ m_State = val; }
	const Actions& GetActions() const		{ return m_Actions; }
    std::size_t TotalBet() const            { return m_TotalBet; }
    void TotalBet(std::size_t val)          { m_TotalBet = val; }
    bool Afk() const                        { return m_Afk; }
    void Afk(bool val)                      { m_Afk = val; }

    const std::vector<float>& Equities() const { return m_Equities; }

    void Reset();

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
    
	bool operator == (const Player& other) const;

	//! Add player action
	void PushAction(unsigned street, Action::Value action, BetSize::Value value, Position::Value pos);

    //! Add player equity
    void PushEquity(float val) { m_Equities.push_back(val); }

private:
	std::string m_Name;		
	std::size_t m_Stack;
	std::size_t m_Bet;				//!< player bet on this street
	Card::List m_Cards;				//!< player cards
	State::Value m_State;			//!< player state
    std::size_t m_TotalBet;         //! player total bet

    Actions m_Actions;				//!< player actions
	std::string m_Country;	
	Styles m_Styles;				//!< player styles during this game
    std::vector<float> m_Equities;  //!< player equities on streets
    bool m_Afk;                     //!< is player away from keyboard
};
}

#endif // Player_h__
