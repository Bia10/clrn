#include "TableLogic.h"
#include "Modules.h"
#include "Exception.h"
#include "Evaluator.h"

#include <boost/make_shared.hpp>

namespace pcmn
{
const unsigned CURRENT_MODULE_ID = Modules::TableLogic;

TableLogic::TableLogic(ILog& logger, ITableLogicCallback& callback)
    : m_Log(logger)
    , m_Callback(callback)
    , m_State(State::Uninited)
    , m_Phase(Phase::Preflop)
{

}

void TableLogic::PushAction(const std::string& name, Action::Value action, unsigned amount)
{
    LOG_TRACE("Name: [%s], action: [%s], amount: [%s]") % name % Action::ToString(action) % amount;
    CHECK(m_Phase <= Phase::River, m_Phase, name, action, amount);

    if (action == Action::SmallBlind)
        ResetPhase(name);

    if 
    (
        action == Action::BigBlind 
        && 
        std::find_if
        (
            m_Actions[Phase::Preflop].begin(), 
            m_Actions[Phase::Preflop].end(),
            [](const ActionDesc& actionDsc) { return actionDsc.m_Value == Action::BigBlind; }
        ) != m_Actions[Phase::Preflop].end()
    )
        return; // big blind duplicated

    ActionDesc actionData(name, action, amount);
    if (m_Actions[m_Phase].empty() || m_Actions[m_Phase].back() != actionData)
        m_Actions[m_Phase].push_back(actionData);

    Player& current = GetPlayer(name);

    switch (action)
    {
    case pcmn::Action::Bet: 
    case pcmn::Action::Raise:
        {
            const unsigned difference = amount - current.Bet();
            current.Bet(current.Bet() + difference);
            current.TotalBet(current.TotalBet() + difference);

            if (m_State != State::Uninited)
            {
                // validate
                CHECK(std::find(m_Sequence.begin(), m_Sequence.end(), name) != m_Sequence.end(), "Failed to find player in sequence", name);

                bool found = false;
                for (std::size_t i = 0; ; ++i)
                {
                    if (i == m_Sequence.size())
                        i = 0;

                    if (!found && m_Sequence[i] == name)
                        found = true;
                    else
                    if (found && m_Sequence[i] == name)
                        break;
                    else
                    if (found)
                    {
                        Player& currentPlayer = GetPlayer(m_Sequence[i]);

                        if (currentPlayer.State() == Player::State::Called)
                        {
                            currentPlayer.State(Player::State::Waiting);
                            m_Queue.push_back(currentPlayer.Name());
                        }
                    }
                }
            }

            if (current.State() != Player::State::AllIn)
                current.State(Player::State::Called);
            break;
        }
    case pcmn::Action::Call: 
        if (current.State() != Player::State::AllIn)
            current.State(Player::State::Called);
        current.Bet(current.Bet() + amount);
        current.TotalBet(current.TotalBet() + amount);
        break;
    case pcmn::Action::SmallBlind:
    case pcmn::Action::BigBlind:
        current.Bet(current.Bet() + amount);
        current.TotalBet(current.TotalBet() + amount);
        break;
    case pcmn::Action::Check:
        current.State(Player::State::Called);
        break;
    case pcmn::Action::Fold:
        current.State(Player::State::Folded);
        break;
    default:
        return;
    }

    if (m_State != State::Uninited)
    {
        CHECK(!m_Queue.empty(), "Unexpected player actions sequence, queue is empty", name);
        CHECK(name == m_Queue.front(), "Incorrect player actions sequence", name, m_Queue.front());
        m_Queue.pop_front();

        if (action == pcmn::Action::SmallBlind)
            PushAction(m_Queue.front(), pcmn::Action::BigBlind, amount * 2);

        if (m_Queue.empty())
        {
            if (m_Phase != Phase::River)
                SetPhase(static_cast<Phase::Value>(m_Phase + 1));
        }
    }

    if (m_State == State::InitedClient && !m_Queue.empty())
    {
        const std::string& botName = Player::ThisPlayer().Name();
        CHECK(!botName.empty(), "Bot name not inited, can't get next");
        if (m_Queue.front() == botName)
            SendRequest();
    }
}

void TableLogic::SetPlayerStack(const std::string& name, unsigned stack)
{
    if (!m_Players.count(name))
        m_Players[name] = boost::make_shared<pcmn::Player>(name, stack);
    else
        GetPlayer(name).Stack(stack);

    if (!stack)
        GetPlayer(name).State(Player::State::AllIn);
}

void TableLogic::SetPlayerCards(const std::string& name, const Card::List& cards)
{
    if (!m_Players.count(name))
        m_Players[name] = boost::make_shared<pcmn::Player>(name, 0);

    GetPlayer(name).Cards(cards);
}

void TableLogic::Parse(const net::Packet& packet)
{

}

void TableLogic::SendRequest()
{

}

void TableLogic::SetPhase(Phase::Value phase)
{
    m_Phase = phase;
    m_Queue.clear();

    ParseActionsIfNeeded();

    CHECK(!m_Sequence.empty(), "Players sequence is empty", m_Phase);
    CHECK(!m_Button.empty(), "Failed to find player on button", m_Phase);

    // add players to queue
    bool found = false;
    for (std::size_t i = 0; ; ++i)
    {
        if (i == m_Sequence.size())
            i = 0;

        Player& current = GetPlayer(m_Sequence[i]);

        if (!found && current.Name() == m_Button)
            found = true;
        else
        if (found && current.Name() == m_Button)
        {
            if (current.State() != Player::State::AllIn && current.State() != Player::State::Folded)
                m_Queue.push_back(current.Name());
            break;
        }
        else
        if (found && current.State() != Player::State::AllIn && current.State() != Player::State::Folded)
            m_Queue.push_back(current.Name());
    }

    if (m_Phase == Phase::Preflop)
    {
        if (m_Sequence.size() > 2)
        {
            // add players on blinds
            const std::string& smallBlind = GetNextPlayerName(m_Button);
            const std::string& bigBlind = GetNextPlayerName(smallBlind);

            if (GetPlayer(smallBlind).Stack())
                m_Queue.push_back(smallBlind);

            if (GetPlayer(bigBlind).Stack())
                m_Queue.push_back(bigBlind);
        }
        else
        {
            m_Queue.push_back(m_Queue.front());
            m_Queue.pop_front();

            if (GetPlayer(m_Button).Stack())
                m_Queue.push_back(m_Button);

            const std::string& next = GetNextPlayerName(m_Button);
            if (GetPlayer(next).Stack())
                m_Queue.push_back(next);
        }
    }

    for (const std::string& player : m_Queue)
    {
        Player& currentPlayer = GetPlayer(player);

        currentPlayer.State(Player::State::Waiting);
        currentPlayer.Bet(0);
    }
}

Player& TableLogic::GetPlayer(const std::string& name)
{
    //CHECK(m_Players.count(name), "Failed to find player by name", name, m_State);
    if (!m_Players.count(name))
        m_Players.insert(std::make_pair(name, boost::make_shared<Player>(name, 1500)));
    return *m_Players[name];
}

Player& TableLogic::GetNextPlayer(const std::string& name)
{
    const std::string& next = GetNextPlayerName(name);
    return GetPlayer(next);
}

const std::string& TableLogic::GetNextPlayerName(const std::string& name) const
{
    CHECK(!m_Sequence.empty(), "Failed to get next player, player queue is empty", name, m_State);

    for (std::size_t i = 0 ; i < m_Sequence.size(); ++i)
    {
        if (m_Sequence[i] != name)
            continue;

        if (i == m_Sequence.size() - 1)
            return m_Sequence.front();
        else
            return m_Sequence[i + 1];
    }

    CHECK(false, "Failed to get next player by name", m_Sequence.size(), name, m_State);
}

const std::string& TableLogic::GetPreviousPlayerName(const std::string& name) const
{
    CHECK(!m_Sequence.empty(), "Failed to get prev player, player queue is empty", name, m_State);

    for (std::size_t i = 0 ; i < m_Sequence.size(); ++i)
    {
        if (m_Sequence[i] != name)
            continue;

        if (!i)
            return m_Sequence.back();
        else
            return m_Sequence[i - 1];
    }

    CHECK(false, "Failed to get prev player by name", m_Sequence.size(), name, m_State);
}


void TableLogic::ResetPhase(const std::string& smallBlind)
{
    if (m_Actions[Phase::Preflop].empty())
        return;

    ParseActionsIfNeeded();

    struct Hand
    {
        std::string m_Player;
        short m_Rank;
    };

    std::vector<Hand> hands;
    std::vector<std::string> allInPlayers;

    // find player hands
    for (const std::string& player : m_Sequence)
    {
        Player& currentPlayer = GetPlayer(player);
        if (currentPlayer.State() == Player::State::AllIn)
            allInPlayers.push_back(player);

        if (currentPlayer.State() == Player::State::Folded)
            continue;

        if (m_Flop.size() != 5 || currentPlayer.Cards().size() != 2)
            continue;

        Card::List cards = m_Flop;
        std::copy(currentPlayer.Cards().begin(), currentPlayer.Cards().end(), std::back_inserter(cards));

        static const Evaluator eval;
        
        const short rank = eval.GetRank(cards);

        const Hand hand = {player, rank};
        hands.push_back(hand);
    }

    m_Flop.clear();

    if (!allInPlayers.empty() && !hands.empty())
    {
        // sort players by rank
        std::sort(hands.begin(), hands.end(), [](const Hand& lhs, const Hand& rhs) { return lhs.m_Rank > rhs.m_Rank; });

        // winner hand
        const Hand& winner = hands.front();

        // find loosers
        for (const Hand& hand : hands)
        {
            if (std::find(allInPlayers.begin(), allInPlayers.end(), hand.m_Player) == allInPlayers.end())
                continue; // player is not all in

            if (hand.m_Player == winner.m_Player || hand.m_Rank == winner.m_Rank)
                continue; // winner or draw

            if (GetPlayer(winner.m_Player).TotalBet() < GetPlayer(hand.m_Player).TotalBet())
                continue; // loose not all money

            // looser
            if (m_Button == hand.m_Player)
                m_Button = GetNextPlayerName(m_Button);

            RemovePlayer(hand.m_Player);
        }
    }

    for (unsigned i = 0 ; i <= Phase::River; ++i)
        m_Actions[i].clear();

    for (const std::string& player : m_Sequence)
    {
        Player& currentPlayer = GetPlayer(player);

        currentPlayer.State(Player::State::Waiting);
        currentPlayer.Bet(0);
        currentPlayer.TotalBet(0);
    }

    if (m_Sequence.size() > 2)
        m_Button = GetPreviousPlayerName(smallBlind);
    else
        m_Button = smallBlind;

    SetPhase(Phase::Preflop);
}

//! Unique add
template<typename C, typename V>
void UniqueAdd(C& c, const V& value)
{
    const typename C::const_iterator it = std::find(c.begin(), c.end(), value);
    if (it == c.end())
        c.push_back(value);
}

void TableLogic::ParseActionsIfNeeded()
{
    if (m_State != State::Uninited)
        return;

    CHECK(m_Queue.empty());
    CHECK(m_Sequence.empty());
    CHECK(!m_Actions[Phase::Preflop].empty());

    // init player queue from actions list
    const ActionDesc::List& actions = m_Actions[Phase::Preflop];

    std::string smallBlind;
    bool bigBlindFound = false;
    for (std::size_t i = 0 ; i < actions.size(); ++i)    
    {
        const ActionDesc& action = actions[i];
        UniqueAdd(m_Sequence, action.m_Name);

        if (!bigBlindFound && action.m_Value == Action::BigBlind)
            bigBlindFound = true;

        if (smallBlind.empty() && action.m_Value == Action::SmallBlind)
            smallBlind = action.m_Name;
        else
        if (!smallBlind.empty() && action.m_Name == smallBlind)
        {
            if (!bigBlindFound)
            {
                CHECK(i < actions.size() - 1, "Failed to find player on big blind, not enough actions", i, actions.size());
                m_Sequence.insert(m_Sequence.begin() + 1, actions[i + 1].m_Name);
            }
            break;
        }
    }

    if (!smallBlind.empty())
        m_Button = GetPreviousPlayerName(smallBlind);
    else
        m_Button = m_Sequence.front(); // parsed by ante, without blinds

    m_State = State::InitedClient;
}

void TableLogic::SetFlopCards(const Card::List& cards)
{
    CHECK(cards.size() >= 3 && cards.size() <= 5, cards.size());
    m_Flop = cards;
}

void TableLogic::RemovePlayer(const std::string& name)
{
    m_Players.erase(name);
    m_Sequence.erase(std::remove(m_Sequence.begin(), m_Sequence.end(), name), m_Sequence.end());
}



}