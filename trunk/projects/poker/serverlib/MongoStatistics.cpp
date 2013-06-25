#include "MongoStatistics.h"
#include "Exception.h"
#include "Config.h"
#include "Modules.h"
#include "Conversion.h"

#include <mongo/client/dbclient.h>
#include <mongo/bson/bsonelement.h>

#include <numeric>

#include <boost/thread/mutex.hpp>
#include <boost/format.hpp>

namespace srv
{
static const int CURRENT_MODULE_ID = Modules::DataBase;

static const char STAT_COLLECTION_NAME[] = "stat.games";

class MongoStatistics::Impl
{
public:

Impl(ILog& logger) : m_Log(logger)
{
	TRY 
	{
        m_Connection.connect("localhost");
	}
	CATCH_PASS_EXCEPTIONS("Failed to init statistics")
}

void Write(pcmn::TableContext::Data& data)
{
	TRY 
	{
		SCOPED_LOG(m_Log);

        // generate board description
        std::vector<bson::bo> board;
        for (const pcmn::Board::Value current : data.m_Board)
        {
            const std::vector<unsigned> street = conv::cast<std::vector<unsigned>>(static_cast<unsigned>(current));
            board.push_back(bson::bob().append("street", street).obj());
        }

        std::vector<bson::bo> players;
        for (const pcmn::Player& player : data.m_PlayersData)
            players.push_back(BuildPlayer(player));

        m_Connection.insert(STAT_COLLECTION_NAME, BSON("players" << players << "flop" << data.m_Flop << "board" << board));
	}
	CATCH_IGNORE_EXCEPTIONS(m_Log, "Failed to write data to database")
}

unsigned GetRanges(PlayerInfo::List& players)
{
	TRY
	{
        // get all games with specified actions on preflop
        SCOPED_LOG(m_Log);

        unsigned count = 0;
        for (PlayerInfo& info : players)
        {
            info.m_CardRange = GetRange(info.m_Actions, info.m_Name);
            if (!info.m_CardRange)
                info.m_CardRange = GetRange(info.m_Actions);
        }
   
        return count;
	}
	CATCH_PASS_EXCEPTIONS("GetRanges failed")
}

pcmn::Player::Style::Value GetAverageStyle(const std::string& target, const std::string& opponent)
{
	TRY
	{
        SCOPED_LOG(m_Log);

        LOG_TRACE("Fetching last actions: [%s], opponent: [%s]") % target % opponent;

        mongo::Query query =             
        BSON("players" << BSON("$all" 
                                    << BSON_ARRAY(
                                            BSON("$elemMatch" 
                                                << BSON("name" << target))
                                            << BSON("$elemMatch" 
                                                << BSON("name" << opponent))
                                        )));
        query.sort("_id", 0);

        // fetch 
        const std::auto_ptr<mongo::DBClientCursor> cursor = m_Connection.query
        (
            STAT_COLLECTION_NAME, 
            query,
            10
        );

        int checkFolds = 0;
        int calls = 0;
        int raises = 0;
        unsigned count = 0;

        while (cursor->more()) 
        {
            unsigned passiveActions = 0;
            unsigned normalActions = 0;
            unsigned aggressiveActions = 0;

            const bson::bo data = cursor->next();
            LOG_TRACE("Fetched last actions: [%s]") % data.toString();

            const std::vector<bson::be> players = data["players"].Array();
            for (const bson::be& elem : players)
            {
                const bson::bo player = elem.Obj();
                if (player["name"].String() != target)
                    continue;

                // found target player, enum all actions
                const std::vector<bson::be> streets = player["streets"].Array();
                for (const bson::be& street : streets)
                {
                    const std::vector<bson::be> actions = street["actions"].Array();
                    for (const bson::be& action : actions)
                    {
                        const pcmn::Action::Value id = static_cast<pcmn::Action::Value>(action["id"].Int());

                        switch (id)
                        {
                        case pcmn::Action::Check:
                        case pcmn::Action::Fold:
                            ++passiveActions;
                            break;
                        case pcmn::Action::Call:
                            ++normalActions;
                            break;
                        case pcmn::Action::Bet:
                        case pcmn::Action::Raise:
                            ++aggressiveActions;
                            break;
                        }
                    }
                }
                break;
            }

            if (aggressiveActions)
                ++raises;
            else
            if (normalActions)
                ++calls;
            else
                ++checkFolds;

            ++count;

            if (count == 4 && raises > 1)
                return pcmn::Player::Style::Aggressive;
        }

        const int summ = checkFolds + calls + raises;
        if (!summ)
            return pcmn::Player::Style::Normal;

        if (raises >= summ / 2)
            return pcmn::Player::Style::Aggressive;

	    if (raises > 1)
		    return pcmn::Player::Style::Normal;

	    return pcmn::Player::Style::Passive;
	}
	CATCH_PASS_EXCEPTIONS("GetLastActions failed")
}

void GetEquities(PlayerInfo::List& players, unsigned street)
{
    TRY
    {
        SCOPED_LOG(m_Log);

        for (PlayerInfo& info : players)
        {
            info.m_WinRate = GetEquity(info.m_Actions, street, info.m_Name);

            if (!info.m_WinRate)
            {
                LOG_TRACE("Failed to find equity by player name: [%s], getting from all statistics") % info.m_Name;
                info.m_WinRate = GetEquity(info.m_Actions, street);
            }
        }
    }
    CATCH_PASS_EXCEPTIONS("GetEquities failed")
}


void GetHands(PlayerInfo::List& players, unsigned street)
{
    TRY
    {
        SCOPED_LOG(m_Log);

        for (PlayerInfo& info : players)
        {
            if (!GetHand(info, street, info.m_Name))
            {
                LOG_TRACE("Failed to find hand by player name: [%s], getting from all statistics") % info.m_Name;
                GetHand(info, street);
            }
        }
    }
    CATCH_PASS_EXCEPTIONS("GetHands failed")
}

private:

bool GetHand(PlayerInfo& info, unsigned streetId, const std::string& name)
{
    SCOPED_LOG(m_Log);

    try
    {
        // return projection
        static const bson::bo returnProjection = bson::bob().append("players.$", 1).append("_id", 0).obj();

        std::map<pcmn::Hand::Value, unsigned> counters;
         
        // prepare list of actions
        const std::vector<bson::bo> actions = GetActions(info.m_Actions);

        const std::string streetsAndActions = std::string("streets.") + conv::cast<std::string>(streetId) + std::string(".actions");
        mongo::Query query =
        BSON("players" << BSON("$elemMatch" 
                            << BSON("name" << name
                            << "cards.0" << BSON("$exists" << 1)
                            << streetsAndActions << BSON("$all" << actions))));

        query.sort("_id", 0);       

        LOG_TRACE("Fetching player hands: [%s]") % query.toString();

        // fetch 
        const std::auto_ptr<mongo::DBClientCursor> cursor = m_Connection.query
        (
            STAT_COLLECTION_NAME, 
            query,
            100,
            0, 
            &returnProjection
        );

        unsigned total = 0;
        while (cursor->more()) 
        {
            const bson::bo data = cursor->next();
            LOG_DEBUG("Fetched player hands: [%s], data: [%s]") % name % data.toString();

            // found target player, enum all actions
            const std::vector<bson::be> streets = data.getFieldDotted("players.0.streets").Array();
            CHECK(streets.size() > streetId, "Bad statistics, returned less streets than expected", streetId, data.toString());

            const pcmn::Hand::Value hand = GetHandFromStreet(streets[streetId]);
            LOG_TRACE("Player: [%s], hand: [%s]") % name % hand;
            ++counters[hand];
            ++total;
        }

        for (const auto& pair : counters)
        {
            // calculate average chance of this hand
            const float avg = static_cast<float>(pair.second) / total;
            info.m_Hands[pair.first] = avg;
        }

        return total > 2;       
    }
    CATCH_PASS_EXCEPTIONS("GetHand failed", name)
}

void GetHand(PlayerInfo& info, unsigned streetId)
{
    try
    {
        // return projection
        static const bson::bo returnProjection = bson::bob().append("players.$", 1).append("_id", 0).obj();

        std::vector<double> equities;

        // query all games with equity info about this player

        // prepare list of actions
        const std::vector<bson::bo> actions = GetActions(info.m_Actions);

        // prepare query
        const std::string streetsAndActions = std::string("streets.") + conv::cast<std::string>(streetId) + std::string(".actions");
        mongo::Query query =
        BSON("players" << BSON("$elemMatch" 
                            << BSON("cards.0" << BSON("$exists" << 1)
                                    << streetsAndActions << BSON("$all"
                                                            << actions))));
        query.sort("_id", 0);

        LOG_TRACE("Fetching player hands: [%s]") % query.toString();

        // fetch 
        const std::auto_ptr<mongo::DBClientCursor> cursor = m_Connection.query
        (
            STAT_COLLECTION_NAME, 
            query,
            100,
            0, 
            &returnProjection
        );

        std::map<pcmn::Hand::Value, unsigned> counters;
        unsigned total = 0;
        while (cursor->more()) 
        {
            const bson::bo data = cursor->next();
            LOG_DEBUG("Fetched all stats equities, data: [%s]") % data.toString();

            const std::vector<bson::be> streets = data.getFieldDotted("players.0.streets").Array();
            CHECK(streets.size() > streetId, "Bad, statistics returned less streets than expected", streetId, data.toString());

            const pcmn::Hand::Value hand = GetHandFromStreet(streets[streetId]);
            LOG_TRACE("Player: [%s], hand: [%s]") % data.getFieldDotted("players.0.name").String() % hand;
            ++counters[hand];
            ++total;
        }

        for (const auto& pair : counters)
        {
            // calculate average chance of this hand
            const float avg = static_cast<float>(pair.second) / total;
            info.m_Hands[pair.first] = avg;
        }
    }
    CATCH_PASS_EXCEPTIONS("GetHand from all stats failed")
}

unsigned GetRange(const pcmn::Player::ActionDesc::List& actionDescs, const std::string& name = std::string())
{
	TRY
	{
        // get all games with specified actions on preflop
        SCOPED_LOG(m_Log);

        // return projection
        static const bson::bo returnProjection = bson::bob().append("players.$", 1).append("_id", 0).obj();

        std::vector<int> cards;

        // query all games with cards info about this player or for all stats

        // prepare list of actions
        const std::vector<bson::bo> actions = GetActions(actionDescs);

        mongo::Query query = 
            name.empty() ? 
        BSON("players" << BSON("$elemMatch" 
                                << BSON("cards" << BSON("$size" << 2)
                                        << "streets.0.actions" << BSON("$all"
                                            << actions))))
            :
        BSON("players" << BSON("$elemMatch" 
                        << BSON("name" << name 
                                << "cards" << BSON("$size" << 2)
                                << "streets.0.actions" << BSON("$all"
                                    << actions))))
            ;

        query.sort("_id", 0);

        // fetch 
        const std::auto_ptr<mongo::DBClientCursor> cursor = m_Connection.query
        (
            STAT_COLLECTION_NAME, 
            query,
            100, 
            0, 
            &returnProjection
        );

        while (cursor->more()) 
        {
            const bson::bo data = cursor->next();
            LOG_TRACE("Fetched player range: [%s], data: [%s]") % name % data.toString();

            const std::vector<bson::be> cardsArray = data.getFieldDotted("players.0.cards").Array();
            for (const bson::be& elem : cardsArray)
                cards.push_back(elem.Int());
        }

        if (cards.size() < 4)
            return 0;

        // find average value
        const unsigned summ = std::accumulate(cards.begin(), cards.end(), 0);
        LOG_TRACE("Player: [%s], cards: [%s], summ: [%s], result: [%s]") % name % cards % summ % (summ / cards.size());
        return summ / cards.size();
	}
	CATCH_PASS_EXCEPTIONS("GetRanges failed")
}

bool IsActionsMatch(const bson::bo& street, const PlayerInfo::Actions& actionsDescs)
{
    // get all actions from street and compare with all actions from outside
    const std::vector<bson::be> actions = street["actions"].Array();

    PlayerInfo::Actions fetched;

    for (const bson::be& action : actions)
    {
        const pcmn::Action::Value id = static_cast<pcmn::Action::Value>(action["id"].Int());
        const pcmn::BetSize::Value amount = static_cast<pcmn::BetSize::Value>(action["amount"].Int());

        const pcmn::Player::ActionDesc desc = {id, amount, pcmn::Player::Position::Max};
        if (actionsDescs.size() == 1 && actionsDescs.front() == desc)
            return true;

        fetched.push_back(desc);
    }

    if (actions.size() == 1)
        return false; // not matched

    for (const PlayerInfo::Actions::value_type& actionDesc : actionsDescs)
    {
        if (std::find(fetched.begin(), fetched.end(), actionDesc) == fetched.end())
            return false;
    }

    return true;
}

float GetEquity(const PlayerInfo::Actions& actionDescs, unsigned streetId, const std::string& name)
{
    try
    {
        // return projection
        static const bson::bo returnProjection = bson::bob().append("players.$", 1).append("_id", 0).obj();

        std::vector<double> equities;

        // query all games with equity info about this player

        // prepare list of actions
        const std::vector<bson::bo> actions = GetActions(actionDescs);

        // prepare query
        mongo::Query query = 
        BSON("players" << BSON("$elemMatch" 
                                << BSON("name" << name
                                        << "equities.0" << BSON("$exists" << 1)
                                        << "streets" << BSON("$elemMatch" 
                                                        << BSON("actions" << BSON("$all"
                                                                << actions))))));

        query.sort("_id", 0);
        

        // fetch 
        const std::auto_ptr<mongo::DBClientCursor> cursor = m_Connection.query
        (
            STAT_COLLECTION_NAME, 
            query,
            10, // ten last games
            0, 
            &returnProjection
        );

        while (cursor->more()) 
        {
            const bson::bo data = cursor->next();
            LOG_TRACE("Fetched player equities: [%s], data: [%s]") % name % data.toString();

            // all equities
            const std::vector<bson::be> equitiesArray = data.getFieldDotted("players.0.equities").Array();
            assert(!equitiesArray.empty());

            // found target player, enum all actions
            const std::vector<bson::be> streets = data.getFieldDotted("players.0.streets").Array();

            if (!streetId) // preflop
            {
                if (IsActionsMatch(streets.front().Obj(), actionDescs))
                    equities.push_back(equitiesArray.front().Double());
            }
            else
            {
                for (unsigned i = 0; i < streets.size(); ++i)
                {
                    if (equitiesArray.size() <= i)
                        break;

                    if (IsActionsMatch(streets[i].Obj(), actionDescs))
                        equities.push_back(equitiesArray[i].Double());
                }
            }
        }

        if (equities.size() == 1)
        {
            const float avg = GetEquity(actionDescs, streetId);
            return static_cast<float>((avg + equities.front()) / 2);
        }

        if (equities.empty())
            return 0;

        // find average value
        const double summ = std::accumulate(equities.begin(), equities.end(), double());
        LOG_TRACE("Player: [%s], equities: [%s], summ: [%s], result: [%s]") % name % equities % summ % static_cast<float>(summ / equities.size());
        return static_cast<float>(summ / equities.size());
    }
    CATCH_PASS_EXCEPTIONS("GetEquity failed", name)
}

float GetEquity(const PlayerInfo::Actions& actionDescs, unsigned street)
{
    try
    {
        // return projection
        static const bson::bo returnProjection = bson::bob().append("players.$", 1).append("_id", 0).obj();

        std::vector<double> equities;

        // query all games with equity info about this player

        // prepare list of actions
        const std::vector<bson::bo> actions = GetActions(actionDescs);

        // prepare query
        const std::string streetsAndActions = std::string("streets.") + conv::cast<std::string>(street) + std::string(".actions");
        mongo::Query query =
        BSON("players" << BSON("$elemMatch" 
                << BSON("equities.0" << BSON("$exists" << 1)
                        << streetsAndActions << BSON("$all"
                                                << actions))));
        query.sort("_id", 0);


        // fetch 
        const std::auto_ptr<mongo::DBClientCursor> cursor = m_Connection.query
        (
            STAT_COLLECTION_NAME, 
            query,
            100,
            0, 
            &returnProjection
        );

        while (cursor->more()) 
        {
            const bson::bo data = cursor->next();
            LOG_TRACE("Fetched all stats equities, data: [%s]") % data.toString();

            const std::vector<bson::be> equitiesArray = data.getFieldDotted("players.0.equities").Array();
            if (equitiesArray.size() > street)
                equities.push_back(equitiesArray[street].Double());
        }

        if (equities.empty())
            return 0;

        // find average value
        const double summ = std::accumulate(equities.begin(), equities.end(), double());
        LOG_TRACE("All stats equities: [%s], summ: [%s], result: [%s]") % equities % summ % static_cast<float>(summ / equities.size());
        return static_cast<float>(summ / equities.size());
    }
    CATCH_PASS_EXCEPTIONS("GetEquity from all stats failed")
}

bson::bo BuildPlayer(const pcmn::Player& player) const
{
    bson::bob builder;

    // player hand description on different streets
    const pcmn::Player::Hands& hands = player.GetHands();

    std::vector<bson::bo> streets;
    for (const pcmn::Player::ActionDesc::List& street : player.GetActions())
    {
        std::vector<bson::bo> actions;
        for (const pcmn::Player::ActionDesc& action : street)
        {
            actions.push_back
            (
                bson::bob()
                    .append("id", action.m_Id)
                    .append("amount", action.m_Amount)
                    .append("position", action.m_Position)
                    .append("reason", action.m_ReasonId)
                    .append("bet", action.m_ReasonAmount).obj()
            );
        }

        std::vector<unsigned> hand;
        if (hands.size() > streets.size()) 
            hand = conv::cast<std::vector<unsigned>>(static_cast<unsigned>(hands[streets.size()]));

        streets.push_back(bson::bob().append("actions", actions).append("hand", hand).obj());
    }

    std::vector<int> cards;
    for (const pcmn::Card& card : player.Cards())
        cards.push_back(card.ToEvalFormat());

    builder
        .append("name", player.Name())
        .append("stack", player.Stack())
        .append("streets", streets)
        .append("cards", cards)
        .append("equities", player.Equities());

    return builder.obj();
}

pcmn::Hand::Value GetHandFromStreet(const bson::be& street)
{
    const std::vector<bson::be> hands = street.Obj()["hand"].Array();
    std::vector<unsigned> bits;
    for (const bson::be& element : hands)
        bits.push_back(element.Int());

    return static_cast<pcmn::Hand::Value>(conv::cast<unsigned>(bits));   
}

std::vector<bson::bo> GetActions(const pcmn::Player::ActionDesc::List& actionsDscs)
{
    std::vector<bson::bo> actions;
    for (const pcmn::Player::ActionDesc& actionDsc : actionsDscs)
    {
        const bson::bo object = BSON("id" << actionDsc.m_Id << "amount" << actionDsc.m_Amount << "position" << actionDsc.m_Position);
        const bson::bo match = BSON("$elemMatch" << object);
        actions.push_back(match);
    }
    return actions;
}

private:
	ILog& m_Log;
	mongo::DBClientConnection m_Connection;
};

MongoStatistics::MongoStatistics(ILog& logger) : m_Impl(new Impl(logger))
{

}

MongoStatistics::~MongoStatistics()
{
	delete m_Impl;
}

void MongoStatistics::Write(pcmn::TableContext::Data& data)
{
	m_Impl->Write(data);
}

unsigned MongoStatistics::GetRanges(PlayerInfo::List& players) const
{
	return m_Impl->GetRanges(players);
}

void MongoStatistics::GetEquities(PlayerInfo::List& players, unsigned street) const
{
	m_Impl->GetEquities(players, street);
}

pcmn::Player::Style::Value srv::MongoStatistics::GetAverageStyle(const std::string& target, const std::string& opponent) const 
{
    return m_Impl->GetAverageStyle(target, opponent);
}

void MongoStatistics::GetHands(PlayerInfo::List& players, unsigned street) const 
{
    m_Impl->GetHands(players, street);
}

}

