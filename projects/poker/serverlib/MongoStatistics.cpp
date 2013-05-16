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

        std::vector<bson::bo> players;
        for (const pcmn::Player& player : data.m_PlayersData)
            players.push_back(BuildPlayer(player));

        m_Connection.insert(STAT_COLLECTION_NAME, BSON("players" << players << "flop" << data.m_Flop));
	}
	CATCH_IGNORE_EXCEPTIONS(m_Log, "Failed to write data to database")
}

unsigned GetRanges(PlayerInfo::List& players)
{
	TRY
	{
        // get all games with specified actions on preflop
        SCOPED_LOG(m_Log);

        // return projection
        static const bson::bo returnProjection = bson::bob().append("players.$", 1).append("_id", 0).obj();

        unsigned count = 0;
        for (PlayerInfo& info : players)
        {
            std::vector<int> cards;

            // query all games with cards info about this player
            const std::string& name = info.m_Name;

            // prepare list of actions
            std::vector<bson::bo> actions;
            for (const pcmn::Player::ActionDesc& actionDsc : info.m_Actions)
            {
                const bson::bo object = BSON("id" << actionDsc.m_Id << "amount" << actionDsc.m_Amount);
                LOG_TRACE("Fetching player range: [%s], actions: [%s]") % name % object.toString();
                actions.push_back(object);
            }

            // fetch 
            const std::auto_ptr<mongo::DBClientCursor> cursor = m_Connection.query
            (
                STAT_COLLECTION_NAME, 
                BSON("players" << BSON("$elemMatch" 
                                        << BSON("name" << name 
                                                << "cards" << BSON("$size" << 2)
                                                << "streets.0.actions" << BSON("$all"
                                                    << actions)))),
                0, 
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
                continue;

            ++count;

            // find average value
            const unsigned summ = std::accumulate(cards.begin(), cards.end(), 0);
            LOG_TRACE("Player: [%s], cards: [%s], summ: [%s], count: [%s]") % name % cards % summ % count;
            info.m_CardRange = summ / cards.size();
        }
   
        return count;
	}
	CATCH_PASS_EXCEPTIONS("GetRanges failed")
}

void GetLastActions(const std::string& target, const std::string& opponent, int& checkFolds, int& calls, int& raises)
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
            20
        );

        while (cursor->more()) 
        {
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
                            ++checkFolds;
                            break;
                        case pcmn::Action::Call:
                            ++calls;
                            break;
                        case pcmn::Action::Bet:
                        case pcmn::Action::Raise:
                            ++raises;
                            break;
                        }
                    }
                }
                break;
            }
        }

	}
	CATCH_PASS_EXCEPTIONS("GetLastActions failed")
}

unsigned GetEquities(PlayerInfo::List& players, unsigned street)
{
    TRY
    {
        SCOPED_LOG(m_Log);

        unsigned count = 0;
        for (PlayerInfo& info : players)
        {
            info.m_WinRate = GetEquity(info.m_Actions, street, info.m_Name);

            if (!info.m_WinRate)
            {
                LOG_TRACE("Failed to find equity by player name: [%s], getting from all statistics") % info.m_Name;
                info.m_WinRate = GetEquity(info.m_Actions, street);
            }

            ++count;
        }
   
        return count;
    }
    CATCH_PASS_EXCEPTIONS("GetEquities failed")
}

private:

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
        std::vector<bson::bo> actions;
        for (const pcmn::Player::ActionDesc& actionDsc : actionDescs)
        {
            const bson::bo object = BSON("id" << actionDsc.m_Id << "amount" << actionDsc.m_Amount);
            LOG_TRACE("Fetching player equities: [%s], actions: [%s]") % name % object.toString();
            actions.push_back(object);
        }

        // prepare query
        mongo::Query query = 
        BSON("players" << BSON("$elemMatch" 
                                << BSON("name" << name
                                        << "equities.0" << BSON("$exists" << 1)
                                        << "streets" << BSON("$elemMatch" 
                                                        << BSON("actions" << BSON("$all"
                                                                << actions))))));
        

        // fetch 
        const std::auto_ptr<mongo::DBClientCursor> cursor = m_Connection.query
        (
            STAT_COLLECTION_NAME, 
            query,
            0,
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

        if (equities.empty())
            return 0;

        // find average value
        const double summ = std::accumulate(equities.begin(), equities.end(), double());
        LOG_TRACE("Player: [%s], equities: [%s], summ: [%s]") % name % equities % summ;
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
        std::vector<bson::bo> actions;
        for (const pcmn::Player::ActionDesc& actionDsc : actionDescs)
        {
            const bson::bo object = BSON("id" << actionDsc.m_Id << "amount" << actionDsc.m_Amount);
            LOG_TRACE("Fetching equities from all stats, actions: [%s]") % object.toString();
            actions.push_back(object);
        }

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
        LOG_TRACE("All stats equities: [%s], summ: [%s]") % equities % summ;
        return static_cast<float>(summ / equities.size());
    }
    CATCH_PASS_EXCEPTIONS("GetEquity from all stats failed")
}

bson::bo BuildPlayer(const pcmn::Player& player) const
{
    bson::bob builder;

    std::vector<bson::bo> streets;
    for (const pcmn::Player::ActionDesc::List& street : player.GetActions())
    {
        std::vector<bson::bo> actions;
        for (const pcmn::Player::ActionDesc& action : street)
            actions.push_back(bson::bob().append("id", action.m_Id).append("amount", action.m_Amount).obj());

        streets.push_back(bson::bob().append("actions", actions).obj());
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

private:
	ILog& m_Log;
	mongo::DBClientConnection m_Connection;
	boost::mutex m_Mutex;
	std::map<std::string, unsigned int> m_Players;
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

void MongoStatistics::GetLastActions(const std::string& target, const std::string& opponent, int& checkFolds, int& calls, int& raises) const
{
	m_Impl->GetLastActions(target, opponent, checkFolds, calls, raises);
}

unsigned MongoStatistics::GetEquities(PlayerInfo::List& players, unsigned street) const
{
	return m_Impl->GetEquities(players, street);
}

}

