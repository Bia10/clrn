#ifndef Params_h__
#define Params_h__

#include "WinRate.h"
#include "StackSize.h"
#include "BetSize.h"
#include "Player.h"
#include "Danger.h"

#include <vector>

namespace sql
{
    class Recordset;
    class IDatabase;
    class IStatement;
}

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

		//! Third parameter - bet size
		pcmn::BetSize::Value m_BetSize;

		//! Fourth parameter - active players count
		pcmn::Player::Count::Value m_ActivePlayers;

		//! Fifth parameter - danger
		pcmn::Danger::Value m_Danger;

		//! Six parameter - bot average style
		pcmn::Player::Style::Value m_BotAverageStyle;

		//! Seven parameter - bot style on previous street
		pcmn::Player::Style::Value m_BotStyle;

		//! Eight parameter - stack size
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

        //! From neuro format
        void FromNeuroFormat(const std::vector<float>& in, const std::vector<float>& out);

		//! Comparison
		bool operator < (const Params& other) const;
		bool operator == (const Params& other) const;

		//! To string
		std::string ToString() const;

		//! Is decision equals
		bool IsDecisionEquals(const Params& other) const;

        //! Get params hash
        unsigned Hash() const;

        //! Get max hash value
        static unsigned MaxHash();

        //! Set params value by hash
        void SetParams(unsigned hash);

        //! Write to db
        void Write(sql::IStatement& stmnt) const;

        //! Read from db
        void Read(sql::Recordset& recordset);

        //! Read from db all params
        static std::size_t ReadAll(List& params, sql::IDatabase& db, const std::string& where = "");

        //! Write to db all params
        static void WriteAll(const List& params, sql::IDatabase& db);
	};

}

#endif // Params_h__
