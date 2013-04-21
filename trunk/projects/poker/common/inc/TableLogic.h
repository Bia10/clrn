#ifndef TableLogic_h__
#define TableLogic_h__

#include "ILog.h"
#include "packet.pb.h"
#include "Player.h"
#include "TableContext.h"
#include "Actions.h"
#include "ITable.h"
#include "Evaluator.h"

#include <map>

#include <boost/noncopyable.hpp>

namespace pcmn
{

//! Table logic callback
class ITableLogicCallback
{
public:

    //! On send data to server
    virtual void SendRequest(const net::Packet& packet, bool statistics) = 0;

    //! On need decision
    virtual void MakeDecision
	(	
		const Player& player, 
		const Player::Queue& activePlayers,
		const TableContext& context,
		const Player::Position::Value position
	) = 0;

    //! Write statistics to db
    virtual void WriteStatistics(TableContext::Data& data) = 0;

};

//! Table logic implementation
class TableLogic : boost::noncopyable
{
public:

    //! Table phase
    struct Phase
    {
        enum Value
        {
            Preflop	= 0,
            Flop = 1,
            Turn = 2,
            River = 3
        };
    };

private:
    //! Players data map
    typedef std::map<std::string, Player> PlayersMap;

    //! Players queue
    typedef std::deque<std::string> PlayerQueue;    

    //! Action description
    struct ActionDesc
    {
        //! Actions list
        typedef std::vector<ActionDesc> List;

        ActionDesc(const std::string& name, pcmn::Action::Value value, unsigned amount)
            : m_Name(name)
            , m_Value(value)
            , m_Amount(amount)
        {}
        bool operator != (const ActionDesc& other) const
        {
            return m_Name != other.m_Name || m_Value != other.m_Value || m_Amount != other.m_Amount;
        }

        std::string m_Name;
        pcmn::Action::Value m_Value;
        unsigned m_Amount;
    };

    //! Logic state
    struct State
    {
        enum Value
        {
            Uninited = 0,
            InitedClient = 1,
            Server = 2
        };
    };



public:
    TableLogic(ILog& logger, ITableLogicCallback& callback, const Evaluator& evaluator);

    //! Push action
    void PushAction(const std::string& name, Action::Value action, unsigned amount);

    //! Set player info
    void SetPlayerStack(const std::string& name, unsigned stack);

    //! Set player cards
    void SetPlayerCards(const std::string& name, const Card::List& cards);

    //! Set flop cards
    void SetFlopCards(const Card::List& cards);

    //! Parse packet
    void Parse(const net::Packet& packet);

    //! Go to another phase
    void SetPhase(Phase::Value phase);

    //! Pack request and send
    void SendRequest(bool statistics);

    //! Remove player
    void RemovePlayer(const std::string& name);

    //! Set data from next round
    void SetNextRoundData(const pcmn::Player::List& players);

private: 

    //! Get player by name
    Player& GetPlayer(const std::string& name);

    //! Get next player
    Player& GetNextPlayer(const std::string& name);

    //! Get next player name
    const std::string& GetNextPlayerName(const std::string& name) const;

    //! Get previous player name
    const std::string& GetPreviousPlayerName(const std::string& name) const;

    //! Rest phase
    void ResetPhase(const std::string& smallBlind);

    //! Parse actions if needed
    void ParseActionsIfNeeded();

    //! ParseFlopCards
    void ParseFlopCards(TableContext& context, const net::Packet& packet);

    //! Parse players
    void ParsePlayers(TableContext& context, const net::Packet& packet);

    //! Get player equity
    std::vector<float> GetPlayerEquities(const int first, const int second, const net::Packet& packet, TableContext& context);

    //! Get active players
    void GetActivePlayers(Player::Queue& players);

    //! Get player position
    Player::Position::Value GetPlayerPosition(const Player::Queue& players, const Player& player);

private:

    //! Logger reference
    ILog& m_Log;

    //! Callback reference
    ITableLogicCallback& m_Callback;

    //! Player data
    PlayersMap m_Players;

    //! Players list
    PlayerQueue m_Sequence;

    //! Players queue
    PlayerQueue m_Queue;

    //! Table actions
    ActionDesc::List m_Actions[Phase::River + 1];

    //! Logic state
    State::Value m_State;

    //! Current phase
    Phase::Value m_Phase;

    //! Flop cards
    Card::List m_Flop;

    //! Player on button
    std::string m_Button;

    //! Hands evaluator
    const Evaluator& m_Evaluator;

    //! Next round data
    pcmn::Player::List m_NextRoundData;

    //! Small blind amount
    unsigned m_SmallBlindAmount;

    //! Pot size
    unsigned m_Pot;

    //! Need decision
    bool m_IsNeedDecision;
};

}


#endif // TableLogic_h__
