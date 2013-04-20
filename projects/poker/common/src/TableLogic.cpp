#include "TableLogic.h"
#include "Modules.h"
#include "Exception.h"
#include "Evaluator.h"
#include "TableContext.h"

namespace pcmn
{
const unsigned CURRENT_MODULE_ID = Modules::TableLogic;

//! Unique add
template<typename C, typename V>
void UniqueAdd(C& c, const V& value)
{
    const typename C::const_iterator it = std::find(c.begin(), c.end(), value);
    if (it == c.end())
        c.push_back(value);
}

TableLogic::TableLogic(ILog& logger, ITableLogicCallback& callback, const Evaluator& evaluator)
    : m_Log(logger)
    , m_Callback(callback)
    , m_State(State::Uninited)
    , m_Phase(Phase::Preflop)
    , m_Evaluator(evaluator)
    , m_SmallBlindAmount(0)
    , m_Pot(0)
{

}

void TableLogic::PushAction(const std::string& name, Action::Value action, unsigned amount)
{
    LOG_TRACE("Name: [%s], action: [%s], amount: [%s]") % name % Action::ToString(action) % amount;
    CHECK(m_Phase <= Phase::River, m_Phase, name, action, amount);

    if (action == Action::SmallBlind)
    {
        m_SmallBlindAmount = amount;
        ResetPhase(name);
    }

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
            if (m_State == State::Server)
                current.Stack(current.Stack() - difference);

            m_Pot += difference;

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

                        if (currentPlayer.State() == Player::State::Called && currentPlayer.Stack())
                        {
                            currentPlayer.State(Player::State::Waiting);
                            m_Queue.push_back(currentPlayer.Name());
                        }
                    }
                }
            }

            current.State(Player::State::Called);
            break;
        }
    case pcmn::Action::Call: 
        m_Pot += amount;
        current.State(Player::State::Called);
        current.Bet(current.Bet() + amount);
        current.TotalBet(current.TotalBet() + amount);
        if (m_State == State::Server)
            current.Stack(current.Stack() - amount);
       break;
    case pcmn::Action::SmallBlind:
    case pcmn::Action::BigBlind:
        m_Pot += amount;
        current.Bet(current.Bet() + amount);
        current.TotalBet(current.TotalBet() + amount);
        if (m_State == State::Server)
            current.Stack(current.Stack() - amount);
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
            SendRequest(false);
    }
}

void TableLogic::SetPlayerStack(const std::string& name, unsigned stack)
{
    if (!m_Players.count(name))
        m_Players[name] = pcmn::Player(name, stack);
    else
        GetPlayer(name).Stack(stack);
}

void TableLogic::SetPlayerCards(const std::string& name, const Card::List& cards)
{
    if (!m_Players.count(name))
        m_Players[name] = pcmn::Player(name, 0);

    GetPlayer(name).Cards(cards);
}

void TableLogic::ParseFlopCards(TableContext& context, const net::Packet& packet)
{
    SCOPED_LOG(m_Log);

    for (int i = 0 ; i < packet.info().cards_size(); ++i)
    {
        context.m_Data.m_Flop.push_back(packet.info().cards(i));
        m_Flop.push_back(Card().FromEvalFormat(packet.info().cards(i)));
    }
}

void TableLogic::ParsePlayers(TableContext& context, const net::Packet& packet)
{
    SCOPED_LOG(m_Log);

    for (int i = 0 ; i < packet.info().players_size(); ++i)
    {
        const net::Packet::Player& player = packet.info().players(i);

        pcmn::TableContext::Data::Player p;
        p.m_Name = player.name();

        if (player.cards_size() == 2)
        {
            pcmn::TableContext::Data::Hand hand;
            hand.m_Cards.push_back(player.cards(0));
            hand.m_Cards.push_back(player.cards(1));
            hand.m_PlayerIndex = i;

            context.m_Data.m_Hands.push_back(hand);

            p.m_Percents = GetPlayerEquities(player.cards(0), player.cards(1), packet, context);
        }

        context.m_Data.m_Players.push_back(p);
        m_Sequence.push_back(player.name());
        CHECK(m_Players.insert(std::make_pair(player.name(), Player(player.name(), player.stack()))).second, "Player name must be unique", player.name());
    }

    m_Button = context.m_Data.m_Players.at(packet.info().button()).m_Name;
}

std::vector<float> TableLogic::GetPlayerEquities(const int first, const int second, const net::Packet& packet, TableContext& context)
{
    SCOPED_LOG(m_Log);

    std::vector<float> result;
    std::vector<int> flop;
    for (int i = 0 ; i < packet.phases_size(); ++i)
    {
        const net::Packet::Phase& phase = packet.phases(i);

        const std::vector<short> ranges(cfg::MAX_EQUITY_PLAYERS, cfg::CARD_DECK_SIZE);

        if (i == 1 && context.m_Data.m_Flop.size() >= 3)
            std::copy(context.m_Data.m_Flop.begin(), context.m_Data.m_Flop.begin() + 3, std::back_inserter(flop));
        else
        if (i && static_cast<int>(context.m_Data.m_Flop.size()) >= 2 + i)
            flop.push_back(context.m_Data.m_Flop[1 + i]);

        result.push_back(m_Evaluator.GetEquity(first, second, flop, ranges));
    }
    return result;
}

void TableLogic::SetNextRoundData(const pcmn::Player::List& players)
{
    m_NextRoundData = players;
}

void TableLogic::GetActivePlayers(Player::Queue& players)
{
    for (const std::string& player : m_Sequence)
    {
        const Player& current = GetPlayer(player);
        if (current.State() != Player::State::Folded)
            UniqueAdd(players, current);
    }
}

Player::Position::Value TableLogic::GetPlayerPosition(const Player::Queue& players, const Player& player)
{
    SCOPED_LOG(m_Log);

    const Player::Queue::const_iterator it = std::find(players.begin(), players.end(), player);
    CHECK(it != players.end(), "Failed to find player in list", player.Name());

    const std::size_t playerIndex = std::distance(players.begin(), it);

    pcmn::Player::Position::Value result = pcmn::Player::Position::Middle;

    std::size_t step = players.size() / 3;
    if (!step)
        step = 1;

    if (playerIndex >= players.size() - step)
        result = pcmn::Player::Position::Later;
    else
    if (playerIndex <= step)
        result = pcmn::Player::Position::Early;

    LOG_TRACE("Players: [%s], index: [%s], step: [%s], result: [%s]") % players.size() % playerIndex % step % result;
    return result;
}

void TableLogic::Parse(const net::Packet& packet)
{
    TRY 
    {
        m_State = State::Server;

        TableContext context;

        // get small blind amount
        const auto it =
        std::find_if
        (
            packet.phases(0).actions().begin(), 
            packet.phases(0).actions().end(),
            [](const net::Packet::Action& actionDsc) { return actionDsc.id() == Action::SmallBlind; }
        );

        m_SmallBlindAmount = 0;
        if (it != packet.phases(0).actions().end())
            m_SmallBlindAmount = it->amount();

        // parse players and flop
        ParseFlopCards(context, packet);
        ParsePlayers(context, packet); 
        m_Pot = 0;
        context.m_BigBlind = m_SmallBlindAmount * 2;

        // simulate logic
        for (Phase::Value phase = Phase::Preflop ; phase < packet.phases_size(); phase = static_cast<Phase::Value>(phase + 1))
        {
            SetPhase(phase);

            for (int i = 0; i < packet.phases(phase).actions_size(); ++i)
            {
                const Action::Value action = static_cast<Action::Value>(packet.phases(phase).actions(i).id());

                // skip useless actions
                if (!Action::IsUseful(action))
                    continue;

                const std::size_t playerIndex = static_cast<std::size_t>(packet.phases(phase).actions(i).player());
                const unsigned amount = static_cast<unsigned>(packet.phases(phase).actions(i).amount());
                const std::string& player = context.m_Data.m_Players.at(playerIndex).m_Name;

                Player& current = GetPlayer(player);
                Player::Queue activePlayers;
                GetActivePlayers(activePlayers);
                
                const Player::Position::Value position = GetPlayerPosition(activePlayers, current);

                PushAction(player, action, amount);

                TableContext::Data::Action resultAction;
                resultAction.m_Action = action;
                resultAction.m_PlayerIndex = playerIndex;
                resultAction.m_Street = phase;
                resultAction.m_PotAmount = m_Pot ? static_cast<float>(amount) / m_Pot : 1;
                resultAction.m_StackAmount = current.Stack() ? static_cast<float>(amount) / current.Stack() : 1;
                resultAction.m_Position = static_cast<int>(position);

                assert(resultAction.m_PotAmount >= 0);
                assert(resultAction.m_StackAmount >= 0);

                current.PushAction(phase, action, resultAction.m_PotAmount);
                context.m_Data.m_Actions.push_back(resultAction);

                if (context.m_MaxBet < amount)
                    context.m_MaxBet = amount;
            }
        }

        Player::Queue activePlayers;
        GetActivePlayers(activePlayers);

        const unsigned size = std::count_if(activePlayers.begin(), activePlayers.end(), [](const Player& player){ return player.Stack(); });

        if (!m_Queue.empty() && size > 1)
        {
            // need a decision
            const Player& current = GetPlayer(m_Queue.front());
            assert(current.Name() == "CLRN");
            const Player::Position::Value position = GetPlayerPosition(activePlayers, current);

            m_Callback.MakeDecision(current, activePlayers, context, position);
        }
        else
        {
            // write statistics here
            m_Callback.WriteStatistics(context.m_Data);
        }
    }
    CATCH_PASS_EXCEPTIONS("Failed to parse packet")
}

void TableLogic::SendRequest(bool statistics)
{
    TRY 
    {
	    if (m_State == State::Server)
            return;
	
	    PlayerQueue playerNames;
	    if (m_State == State::InitedClient)
	        playerNames = m_Sequence;
	    else
	    {
	        for (std::size_t i = 0 ; i < m_Actions[Phase::Preflop].size(); ++i)    
	        {
	            const ActionDesc& action = m_Actions[Phase::Preflop][i];
	            UniqueAdd(playerNames, action.m_Name);
	        }
	
	        const std::string& botName = Player::ThisPlayer().Name();
	        CHECK(!botName.empty());
	        UniqueAdd(playerNames, botName);
	        m_Button = playerNames.back();
	    }
	
	    net::Packet packet;
	
	    net::Packet::Table& table = *packet.mutable_info();
	
	    for (const pcmn::Card& card : m_Flop)
	        table.mutable_cards()->Add(card.ToEvalFormat());	
	
	    std::size_t counter = 0;
	    std::map<std::string, std::size_t> players;
	    for (const std::string& playerName : playerNames)
	    {
	        const Player& player = GetPlayer(playerName);
	
	        net::Packet::Player& added = *table.add_players();
	        added.set_bet(player.Bet());
	        added.set_name(playerName);
	
	        const unsigned stack = player.Stack() + player.TotalBet();
	        added.set_stack(stack);
	
	        const pcmn::Card::List& cards = player.Cards();
	        for (const pcmn::Card& card : cards)
	            added.mutable_cards()->Add(card.ToEvalFormat());
	
	        if (playerName == m_Button)
	            table.set_button(counter);
	
	        players[playerName] = counter;
	        ++counter;
	    }
	
	    for (unsigned i = 0 ; i <= Phase::River; ++i)
	    {
	        const ActionDesc::List& actions = m_Actions[i];
	        if (actions.empty())
	            break;
	
	        net::Packet::Phase& phase = *packet.add_phases();
	        for (const ActionDesc& dsc : actions)
	        {
	            net::Packet::Action& action = *phase.add_actions();
	            action.set_amount(dsc.m_Amount);
	            action.set_id(dsc.m_Value);
	            action.set_player(players[dsc.m_Name]);
	        }
	    }

        m_Callback.SendRequest(packet, statistics);
    }
    CATCH_PASS_EXCEPTIONS("Failed to send request by table logic")
}

void TableLogic::SetPhase(Phase::Value phase)
{
    m_Phase = phase;
    m_Queue.clear();

    ParseActionsIfNeeded();

    CHECK(!m_Sequence.empty(), "Players sequence is empty", m_Phase);
    CHECK(!m_Button.empty(), "Failed to find player on button", m_Phase);
    CHECK(std::find(m_Sequence.begin(), m_Sequence.end(), m_Button) != m_Sequence.end(), "Failed to find player on button in sequence", m_Button);

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
            if (current.Stack() && current.State() != Player::State::Folded)
                m_Queue.push_back(current.Name());
            break;
        }
        else
        if (found)
        {
            if (current.Stack() && current.State() != Player::State::Folded)
                m_Queue.push_back(current.Name());
            else
            if (m_Phase == Phase::Preflop && m_Queue.size() < 2) // workaround in case when small blind is all in
                m_Queue.push_back(current.Name());
        }
    }

    if (m_Phase == Phase::Preflop)
    {
        // workaround, client doesn't know real stack size at preflop moment
        const unsigned smallBlindValue = State::Server == m_State ? m_SmallBlindAmount : 0;

        if (m_Sequence.size() > 2)
        {
            // add players on blinds
            const std::string& smallBlind = GetNextPlayerName(m_Button);
            const std::string& bigBlind = GetNextPlayerName(smallBlind);

            const Player& smallBlindPlayer = GetPlayer(smallBlind);
            if (smallBlindPlayer.Stack() + smallBlindPlayer.TotalBet() > smallBlindValue)
                m_Queue.push_back(smallBlind);

            const Player& bigBlindPlayer = GetPlayer(bigBlind);
            if (bigBlindPlayer.Stack() + bigBlindPlayer.TotalBet() > smallBlindValue * 2)
                m_Queue.push_back(bigBlind);
        }
        else
        {
            m_Queue.push_back(m_Queue.front());
            m_Queue.pop_front();

            const Player& smallBlindPlayer = GetPlayer(m_Button);
            if (smallBlindPlayer.Stack() + smallBlindPlayer.TotalBet() > smallBlindValue)
                m_Queue.push_back(m_Button);

            const std::string& next = GetNextPlayerName(m_Button);
            const Player& bigBlindPlayer = GetPlayer(next);
            if (bigBlindPlayer.Stack() + bigBlindPlayer.TotalBet() > smallBlindValue * 2)
                m_Queue.push_back(next);
        }
    }

    for (const std::string& player : m_Queue)
    {
        Player& currentPlayer = GetPlayer(player);

        currentPlayer.State(Player::State::Waiting);
        currentPlayer.Bet(0);
    }

    if (m_State == State::InitedClient && !m_Queue.empty() && m_Phase != Phase::Preflop)
    {
        // ignore on preflop
        const std::string& botName = Player::ThisPlayer().Name();
        CHECK(!botName.empty(), "Bot name not inited, can't get next");
        if (m_Queue.front() == botName)
            SendRequest(false);
    }
}

Player& TableLogic::GetPlayer(const std::string& name)
{
    if (!m_Players.count(name))
        m_Players.insert(std::make_pair(name, Player(name, 1500)));
    return m_Players[name];
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
    SendRequest(true);

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
        if (!currentPlayer.Stack())
            allInPlayers.push_back(player);

        if (currentPlayer.State() == Player::State::Folded)
            continue;

        if (m_Flop.size() != 5 || currentPlayer.Cards().size() != 2)
            continue;

        Card::List cards = m_Flop;
        std::copy(currentPlayer.Cards().begin(), currentPlayer.Cards().end(), std::back_inserter(cards));
       
        const short rank = m_Evaluator.GetRank(cards);

        const Hand hand = {player, rank};
        hands.push_back(hand);
    }

    m_Flop.clear();

    if (allInPlayers.size() == 1 && hands.empty())
    {
        // workaround in case when all players folds we must set player stack back
        Player& currentPlayer = GetPlayer(allInPlayers.front());
        if (!currentPlayer.Stack())
            currentPlayer.Stack(currentPlayer.TotalBet());
    }

    if (!allInPlayers.empty() && !hands.empty())
    {
        // sort players by rank
        std::sort(hands.begin(), hands.end(), [](const Hand& lhs, const Hand& rhs) { return lhs.m_Rank > rhs.m_Rank; });

        // winner hand
        const Hand& winner = hands.front();

        // find loosers
        for (const Hand& hand : hands)
        {
            bool skip = false;
            if (std::find(allInPlayers.begin(), allInPlayers.end(), hand.m_Player) == allInPlayers.end())
                skip = true; // player is not all in

            if (hand.m_Player == winner.m_Player || hand.m_Rank == winner.m_Rank)
                skip = true; // winner or draw

            if (GetPlayer(winner.m_Player).TotalBet() < GetPlayer(hand.m_Player).TotalBet())
                skip = true; // loose not all money

            // workaround, all in players may win without info about stacks, so they haven't stack and will be assumed as all in again
            if (skip)
            {
                if (!GetPlayer(hand.m_Player).Stack())
                    GetPlayer(hand.m_Player).Stack(m_SmallBlindAmount * 2 + 1);
                continue;
            }

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

    // set data from this round
    for (const pcmn::Player& player : m_NextRoundData)
        SetPlayerStack(player.Name(), player.Stack());

    m_NextRoundData.clear();
    m_Pot = 0;

    SetPhase(Phase::Preflop);
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