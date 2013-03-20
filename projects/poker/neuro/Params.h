#ifndef Params_h__
#define Params_h__

#include "WinRate.h"
#include "StackSize.h"
#include "BetSize.h"
#include "Player.h"
#include "Danger.h"

#include <vector>

namespace neuro
{
	struct Params
	{
		typedef std::vector<Params> List;

		Params();

		//! First parameter - win rate
		pcmn::WinRate::Value m_WinRate;

		//! Second parameter - position
		pcmn::Player::Position::Value m_Position;

		//! Third parameter - bet pot size
		pcmn::BetSize::Value m_BetPotSize;

		//! Fourth parameter - bet stack size
		pcmn::BetSize::Value m_BetStackSize;

		//! Fifth parameter - active players count
		pcmn::Player::Count::Value m_ActivePlayers;

		//! Six parameter - danger
		pcmn::Danger::Value m_Danger;

		//! Seven parameter - bot average style
		pcmn::Player::Style::Value m_BotAverageStyle;

		//! Eight parameter - bot style on previous street
		pcmn::Player::Style::Value m_BotStyle;

		//! Nine parameter - stack size
		pcmn::StackSize::Value m_BotStackSize;

		//! First out parameter - check/fold action
		bool m_CheckFold;

		//! Second out parameter - check/call action
		bool m_CheckCall;

		//! Third out parameter - bet/raise action
		bool m_BetRaise;

		//! Serialize
		void Serialize(std::ostream& stream) const;

		//! Deserialize
		std::vector<int>::const_iterator Deserialize(std::vector<int>::const_iterator it);

		//! To neuro format
		void ToNeuroFormat(std::vector<float>& in, std::vector<float>& out) const;
	};

}

#endif // Params_h__
