#include "Cards.h"
#include "Actions.h"
#include "BetSize.h"
#include "Player.h"

#include <mongo/client/dbclient.h>
#include <mongo/bson/bsonelement.h>

#include <exception>
#include <iostream>
#include <map>

#include <boost/filesystem.hpp>


struct ActionDsc
{
    typedef std::vector<ActionDsc> List;
    std::string m_Name;
    pcmn::Action::Value m_Id;
    pcmn::BetSize::Value m_Amount;
};

typedef std::vector<ActionDsc::List> Actions;

bool ExtractAction(const bson::bo& game, const std::string& name, const unsigned street, const unsigned actionCount, ActionDsc& result)
{
    ActionDsc tmp = {name, pcmn::Action::Bet, pcmn::BetSize::Low};
    result = tmp;

    const std::vector<bson::be> players = game.getField("players").Array();

    for (const bson::be& player : players)
    {
        const std::string currentName = player.Obj()["name"].String();
        if (currentName != name)
            continue;

        // extract all streets
        const std::vector<bson::be> streets = player.Obj().getField("streets").Array();
        for (unsigned streetIndex = 0; streetIndex < streets.size(); ++streetIndex)
        {
            if (streetIndex != street)
                continue;

            const bson::be& street = streets[streetIndex];
            const std::vector<bson::be> actions = street.Obj().getField("actions").Array();

            for (unsigned i = 0 ; i < actions.size(); ++i)
            {
                if (i != actionCount)
                    continue;

                const bson::be& action = actions[i];
                const pcmn::Action::Value id = static_cast<pcmn::Action::Value>(action["id"].Int());
                const pcmn::BetSize::Value amount = static_cast<pcmn::BetSize::Value>(action["amount"].Int());

                result.m_Id = id;
                result.m_Amount = amount;

                return true;
            }
        }
    }

    return false;
}

int main(int argc, char* argv[])
{
	try 
	{
        mongo::DBClientConnection c;
        c.connect("localhost");

        // fetch all games
        const std::auto_ptr<mongo::DBClientCursor> cursor = c.query("stat.games", bson::bo());
        while (cursor->more()) 
        {
            const bson::bo game = cursor->next();

            Actions actionsData;

            std::list<std::string> sequence;

            // get all players
            const std::vector<bson::be> players = game.getField("players").Array();
            for (const bson::be& player : players)
                sequence.push_back(player.Obj()["name"].String());

            for (unsigned street = 0; street < 4; ++street)
            {
                // all action on this street
                ActionDsc::List actions;

                // called players
                std::vector<std::string> called;

                std::list<std::string> queue = sequence;

                if (!street)
                {
                    actions.resize(2);

                    // small and big blinds
                    actions[0].m_Id = pcmn::Action::SmallBlind;
                    actions[0].m_Amount = pcmn::BetSize::Low;
                    actions[0].m_Name = sequence.front();

                    actions[1].m_Id = pcmn::Action::BigBlind;
                    actions[1].m_Amount = pcmn::BetSize::Low;
                    actions[1].m_Name = *(++sequence.begin());

                    // add blinds to the end
                    queue.push_back(actions[0].m_Name);
                    queue.push_back(actions[1].m_Name);

                    // remove blinds from begin because we just made blinds actions
                    queue.pop_front();
                    queue.pop_front();
                }

                std::map<std::string, unsigned> actionsCounters;
                auto it = queue.begin();

                for (; ; )
                {
                    if (queue.empty())
                        break;

                    // extract action from database data
                    ActionDsc action;
                    const bool success = ExtractAction(game, *it, street, actionsCounters[*it]++, action);
                    if (!success)
                    {
                        // probably this player all in
                        sequence.erase(std::find(sequence.begin(), sequence.end(), *it));
                        it = queue.erase(it);
                        continue;
                    }

                    actions.push_back(action);

                    if (action.m_Id == pcmn::Action::Fold)
                    {
                        sequence.erase(std::find(sequence.begin(), sequence.end(), *it));
                    }
                    else
                    if (action.m_Id == pcmn::Action::Bet || action.m_Id == pcmn::Action::Raise)
                    {
                        for (const std::string& name : called)
                            queue.push_back(name);
                        called.clear();
                        called.push_back(*it);
                    }
                    else
                    {
                        called.push_back(*it);
                    }

                    it = queue.erase(it);
                }

                actionsData.push_back(actions);
            }
        }
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}


