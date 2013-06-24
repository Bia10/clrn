#ifndef TableContext_h__
#define TableContext_h__

#include "Actions.h"
#include "Player.h"
#include "Board.h"

namespace pcmn
{
struct TableContext
{
	//! Packet parsed data
	struct Data
	{
		//! Player info
		struct Player
		{
			typedef std::vector<Player> List;
			std::string m_Name;
			unsigned int m_Index; // row id from sqlite 
			std::vector<float> m_Percents; // player percents on different streets
		};

		struct Action
		{
			typedef std::vector<Action> List;
			std::size_t m_PlayerIndex; // reference to one of the m_Players(index in the vector)
			int m_Street;
			int m_Action;
            int m_Bet;
			int m_Position;
		};

		//! Player hand
		struct Hand
		{
			typedef std::vector<Hand> List;
			int m_PlayerIndex;			// index in the players vector
			std::vector<int> m_Cards;
		};

		Player::List m_Players;
		Action::List m_Actions;
		Hand::List m_Hands;	

        //! Table flop cards
		std::vector<int> m_Flop;

        //! List of players with complete actions and hands
        pcmn::Player::List m_PlayersData;

        //! Board description on different streets
        std::vector<pcmn::Board::Value> m_Board;
	};

	TableContext()
		: m_MaxBet(0)
		, m_Pot(0)
		, m_BigBlind(0)
        , m_Street(0)
	{

	}

	unsigned m_MaxBet;
	unsigned m_Pot;
	unsigned m_BigBlind;
    unsigned m_Street;
	Data m_Data;
};
}


#endif // TableContext_h__
