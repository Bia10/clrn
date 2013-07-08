#include "Cards.h"
#include "Actions.h"
#include "BetSize.h"
#include "Player.h"
#include "TableContext.h"
#include "../../serverlib/MongoStatistics.h"
#include "Log.h"
#include "Modules.h"

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

std::string g_BigBlind;
std::list<std::string> g_LastSequence;

void MakeValidSequence(std::list<std::string>& sequence, ActionDsc::List& actions)
{
    // find out if any aggression exists
    const ActionDsc::List::const_iterator it = std::find_if
    (
        actions.begin(),
        actions.end(),
        [](const ActionDsc& action) { return action.m_Id == pcmn::Action::Raise; }
    );

    if (it == actions.end())
    {
        // ok, preflop contains only passive actions
        // let's find out last 'check', that will be a big blind
        const auto lastCheck = std::find_if
        (
            actions.rbegin(),
            actions.rend(),
            [](const ActionDsc& action) { return action.m_Id == pcmn::Action::Check; }
        );   

        if (lastCheck == actions.rend())
            return; // this round completes without any action(big blind win)

        g_BigBlind = lastCheck->m_Name;

        // small bind is next after the button, so small blind must be first in sequence
        // make player on big blind second to complete this condition
        while (*(++sequence.begin()) != g_BigBlind)
        {
            const std::string temp = sequence.front();
            sequence.pop_front();
            sequence.push_back(temp);
        }

        // ok, now sequence is valid
        g_LastSequence = sequence;
    }
    else
    {
        // preflop contains some aggression
        auto lastBigBlind = std::find(g_LastSequence.begin(), g_LastSequence.end(), g_BigBlind);
        auto player = std::find(sequence.begin(), sequence.end(), g_BigBlind);

        if (player == sequence.end())
        {
            if (lastBigBlind != g_LastSequence.end())
            {
                ++lastBigBlind;
                if (lastBigBlind == g_LastSequence.end())
                    lastBigBlind = g_LastSequence.begin();

                player = std::find(sequence.begin(), sequence.end(), *lastBigBlind);
            }
        }
        else
        {
            ++player;
            if (player == sequence.end())
                player = sequence.begin();
        }

        if (player == sequence.end())
        {
            // failed to find next big blind
        }
        else
        {
            g_BigBlind = *player;

            while (*(++sequence.begin()) != g_BigBlind)
            {
                const std::string temp = sequence.front();
                sequence.pop_front();
                sequence.push_back(temp);
            }
        }
    }
}

pcmn::Player::Position::Value GetPlayerPosition(const unsigned phase, const std::list<std::string>& sequence, const std::list<std::string>& queue)
{
    const unsigned totalPlayers = sequence.size();
    unsigned leftInQueue = queue.size();

    if (!phase && sequence.size() > 3)
    {
        if (leftInQueue <= 2)
            leftInQueue = 0;
        else
            leftInQueue -= 2; // don't calculate blinds
    }

    pcmn::Player::Position::Value result = pcmn::Player::Position::Middle;

    float step = static_cast<float>(sequence.size()) / 3;
    if (step < 1)
        step = 1;

    if (leftInQueue <= step)
        result = pcmn::Player::Position::Later;
    else
        if (leftInQueue >= step * 2.5f)
            result = pcmn::Player::Position::Early;

    return result;
}

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
            std::vector<bson::be> actions = street.Obj().getField("actions").Array();

            for (unsigned i = 0 ; i < actions.size(); ++i)
            {
                if (i != actionCount)
                    continue;

                bson::be& action = actions[i];
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

        Log log;
        log.Open("1", Modules::Server, ILog::Level::Error);
        log.Open("1", Modules::DataBase, ILog::Level::Error);

        srv::MongoStatistics stats(log);

        // fetch all games
        unsigned counter = 0;
        const std::auto_ptr<mongo::DBClientCursor> cursor = c.query("stat.input", bson::bo());
        while (cursor->more()) 
        {
            ++counter;
            const bson::bo game = cursor->next();

            Actions actionsData;
            pcmn::TableContext::Data gameData;
            std::list<std::string> sequence;
            pcmn::Card::List flop;

            // parse flop cards
            const std::vector<bson::be> cardsElements = game["flop"].Array();
            for (const bson::be& card : cardsElements)
            {
                gameData.m_Flop.push_back(static_cast<int>(card.Double()));
                flop.push_back(pcmn::Card().FromEvalFormat(gameData.m_Flop.back()));
            }

            if (flop.size() >= 3)
            {
                for (unsigned i = 3; i <= flop.size(); ++i)
                {
                    const pcmn::Card::List board(flop.begin(), flop.begin() + i);

                    pcmn::Board parser(board);
                    parser.Parse();
                    gameData.m_Board.push_back(parser.GetValue());
                }
            }

            // get all players
            const std::vector<bson::be> players = game.getField("players").Array();
            for (const bson::be& player : players)
            {
                sequence.push_back(player.Obj()["name"].String());
                gameData.m_PlayersData.push_back(pcmn::Player(sequence.back(), player.Obj()["stack"].Int()));

                pcmn::Player& current = gameData.m_PlayersData.back();

                pcmn::Card::List cards;
                const std::vector<bson::be> cardsElements = player.Obj()["cards"].Array();
                for (const bson::be& card : cardsElements)
                    cards.push_back(pcmn::Card().FromEvalFormat(static_cast<int>(card.Double())));

                if (!cards.empty())
                {
                    pcmn::Hand hand;
                    static const pcmn::Card::List empty;
                    hand.Parse(cards, empty);

                    current.Cards(cards);
                    current.PushHand(hand.GetValue());

                    if (flop.size() >= 3)
                    {
                        for (unsigned i = 3; i <= flop.size(); ++i)
                        {
                            const pcmn::Card::List board(flop.begin(), flop.begin() + i);
                            hand.Parse(cards, board);
                            current.PushHand(hand.GetValue());
                        }
                    }
                }

                const std::vector<bson::be> equitiesElements = player.Obj()["equities"].Array();
                for (const bson::be& eq : equitiesElements)
                    current.PushEquity(static_cast<float>(eq.Double()));
            }

            std::list<std::string> backup = sequence;
            bool valid = false;
            for (unsigned street = 0; street < 4; ++street)
            {
                if (!valid && street)
                {
                    // rerun preflop
                    valid = true;
                    street = 0;
                }

                // all action on this street
                ActionDsc::List actions;

                // called players
                std::vector<std::string> called;

                std::list<std::string> queue = sequence;

                if (!street && valid)
                {
                    actions.resize(2);

                    // remove blinds
                    const std::string smallBlind = queue.front();
                    queue.pop_front();
                    const std::string bigBlind = queue.front();
                    queue.pop_front();

                    // small and big blinds
                    actions[0].m_Id = pcmn::Action::SmallBlind;
                    actions[0].m_Amount = pcmn::BetSize::Low;
                    actions[0].m_Name = smallBlind;

                    actions[1].m_Id = pcmn::Action::BigBlind;
                    actions[1].m_Amount = pcmn::BetSize::Low;
                    actions[1].m_Name = bigBlind;

                    // add blinds to the end
                    queue.push_back(smallBlind);
                    queue.push_back(bigBlind);
                }

                std::map<std::string, unsigned> actionsCounters;
                auto it = queue.begin();

                pcmn::Action::Value lastAction = street ? pcmn::Action::Unknown : pcmn::Action::BigBlind;
                pcmn::BetSize::Value lastAmount = street ? pcmn::BetSize::NoBet : pcmn::BetSize::Low;

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

                    const pcmn::Player::Position::Value position = GetPlayerPosition(street, sequence, queue);

                    const pcmn::Player::List::iterator resultPlayer = std::find_if(gameData.m_PlayersData.begin(), gameData.m_PlayersData.end(), [&](const pcmn::Player& p){ return p.Name() == *it; });
                    assert(resultPlayer != gameData.m_PlayersData.end());

                    actions.push_back(action);

                    if (action.m_Id == pcmn::Action::Fold)
                    {
                        sequence.erase(std::find(sequence.begin(), sequence.end(), *it));
                    }
                    else
                    if (action.m_Id == pcmn::Action::Bet || action.m_Id == pcmn::Action::Raise)
                    {
                        if (action.m_Id > lastAction || action.m_Amount > lastAmount)
                        {
                            lastAction = action.m_Id;
                            lastAmount = action.m_Amount;
                        }

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

                    const pcmn::Player::Count::Value count = pcmn::Player::Count::FromValue(sequence.size());

                    if (action.m_Id == pcmn::Action::Check && (lastAction == pcmn::Action::Bet || lastAction == pcmn::Action::Raise))
                        resultPlayer->PushAction(street, action.m_Id, action.m_Amount, position, street ? pcmn::Action::Unknown : pcmn::Action::BigBlind, pcmn::BetSize::NoBet, count);
                    else
                        resultPlayer->PushAction(street, action.m_Id, action.m_Amount, position, lastAction, lastAmount, count);
                }

                if (!street && !valid)
                {
                    MakeValidSequence(backup, actions);
                    sequence = backup;
                }
                else
                    actionsData.push_back(actions);
            }

            stats.Write(gameData);

            if (!(counter % 100))
                std::cout << "Iteration: " << counter << std::endl;
        }
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}


