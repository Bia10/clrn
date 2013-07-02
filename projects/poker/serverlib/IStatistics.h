#ifndef IStatistics_h__
#define IStatistics_h__

#include "ILog.h"
#include "TableContext.h"
#include "Actions.h"
#include "Config.h"
#include "Player.h"
#include "Hand.h"

#include <vector>
#include <string>
#include <map>

namespace srv
{

//! Stats writer/reader
class IStatistics
{
public:

	//! Player info
	struct PlayerInfo
	{
		typedef pcmn::Player::ActionDesc::List Actions;
        typedef std::map<pcmn::Hand::Value, float> Hands;
        typedef std::map<pcmn::Action::Value, float> ActionsMap;
        typedef std::map<pcmn::BetSize::Value, ActionsMap> BetsMap;
		typedef std::vector<PlayerInfo> List;

		PlayerInfo() 
            : m_Bet(0)
            , m_WinRate(0)
            , m_CardRange(cfg::CARD_DECK_SIZE)
            , m_Position()
		{
		}

		std::string m_Name;
		Actions m_Actions;
        int m_Bet;
		float m_WinRate;
		int m_CardRange;
        Hands m_Hands;
        pcmn::Player::Position::Value m_Position;
        BetsMap m_Bets;
	};

	virtual ~IStatistics() {}

	//! Write statistics data
	virtual void Write(pcmn::TableContext::Data& data) = 0;

	//! Get card ranges
	virtual unsigned GetRanges(PlayerInfo::List& players) const = 0;

	//! Get last actions
	virtual pcmn::Player::Style::Value GetAverageStyle(const std::string& target, const std::string& opponent) const = 0;

	//! Get equities
	virtual void GetEquities(PlayerInfo::List& players, unsigned street) const = 0;

    //! Get possible hands by actions
    virtual void GetHands(PlayerInfo& player, unsigned street, unsigned count) const = 0;

    //! Get actions by board description and player info
    virtual void GetActions(PlayerInfo& player, pcmn::Board::Value board, unsigned street, unsigned count) const = 0;
};

} // namespace srv

#endif // IStatistics_h__
