#include "TableLogic.h"
#include "Modules.h"
#include "Exception.h"
#include "Evaluator.h"
#include "TableContext.h"
#include "BetSize.h"

#include <boost/assign/list_of.hpp>

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
    , m_IsNeedDecision(false)
    , m_IsRoundFinished(false)
{

}

void TableLogic::PushAction(const std::string& name, Action::Value action, unsigned amount)
{
    try
    {
        LOG_TRACE("Name: [%s], action: [%s], amount: [%s]") % name % Action::ToString(action) % amount;
        CHECK(m_Phase <= Phase::River, m_Phase, name, action, amount);

        const bool resetOnly = (m_State == State::Uninited && action == Action::SmallBlind);
        if (resetOnly || m_IsRoundFinished && Action::IsUseful(action))
        {
            m_IsRoundFinished = true;
            if (!resetOnly)
                SendRequest(true);

            ResetPhase();

            if (!m_Queue.empty())
            {
                PushAction(m_Queue.front(), pcmn::Action::SmallBlind, m_Context.m_BigBlind / 2);
                PushAction(m_Queue.front(), pcmn::Action::BigBlind, m_Context.m_BigBlind);
            }
        }

        if (action == Action::SmallBlind && FindExistingSmallBlind(name, amount))
            return;

        ActionDesc actionData(name, action, amount);
        if (!m_Actions[m_Phase].empty() && m_Actions[m_Phase].back() == actionData)
            return; // duplicated

        m_Actions[m_Phase].push_back(actionData);

        Player& current = GetPlayer(name);
        if (current.Afk() && Action::IsUseful(action) && action != Action::SmallBlind && action != Action::BigBlind && action != Action::Fold)
            current.Afk(false);

        switch (action)
        {
        case pcmn::Action::Bet: 
        case pcmn::Action::Raise:
            {
                const unsigned difference = amount - current.Bet();
                current.Bet(current.Bet() + difference);
                current.TotalBet(current.TotalBet() + difference);
                if (m_State == State::Server)
                {
                    assert(current.Stack() >= difference);
                    current.Stack(current.Stack() - difference);
                }

                m_Context.m_Pot += difference;

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
                                LOG_TRACE("Adding to queue: [%s], state: [%s], stack: [%s]") % currentPlayer.Name() % currentPlayer.State() % currentPlayer.Stack();
                                currentPlayer.State(Player::State::Waiting);
                                m_Queue.push_back(currentPlayer.Name());
                            }
                            else
                                LOG_TRACE("Skipping add to queue: [%s], state: [%s], stack: [%s]") % currentPlayer.Name() % currentPlayer.State() % currentPlayer.Stack();
                        }
                    }
                }

                current.State(Player::State::Called);
                break;
            }
        case pcmn::Action::Call: 
            m_Context.m_Pot += amount;
            current.State(Player::State::Called);
            current.Bet(current.Bet() + amount);
            current.TotalBet(current.TotalBet() + amount);
            if (m_State == State::Server)
            {
                assert(current.Stack() >= amount);
                current.Stack(current.Stack() - amount);
            }
            break;
        case pcmn::Action::SmallBlind:
            m_Context.m_BigBlind = amount * 2;
        case pcmn::Action::BigBlind:
            m_Context.m_Pot += amount;
            current.Bet(current.Bet() + amount);
            current.TotalBet(current.TotalBet() + amount);
            if (m_State == State::Server)
            {
                assert(current.Stack() >= amount);
                current.Stack(current.Stack() - amount);
            }
            break;
        case pcmn::Action::Check:
            current.State(Player::State::Called);
            break;
        case pcmn::Action::Fold:
            current.State(Player::State::Folded);
            if (amount == 1)
                current.Afk(true);
            break;
        case pcmn::Action::Win:
            // if we reached here some players are not folded, we must add fold actions for them
            while (m_Queue.size() > 1)
            {
                const unsigned size = m_Queue.size();
                PushAction(m_Queue.front(), Action::Fold, 0);
                if (size == m_Queue.size())
                    m_Queue.pop_front();
            }
            return;
        default:
            return;
        }          

        bool phaseChanged = false;
        if (m_State != State::Uninited)
        {
            CHECK(!m_Queue.empty(), "Unexpected player actions sequence, queue is empty", name);
            if (name != m_Queue.front())
            {
                while (GetPlayer(m_Queue.front()).Afk())
                {
                    const unsigned size = m_Queue.size();
                    PushAction(m_Queue.front(), Action::Fold, 0);
                    if (size == m_Queue.size())
                        m_Queue.pop_front();
                    std::swap(*(m_Actions[m_Phase].end() - 1), *(m_Actions[m_Phase].end() - 2));
                }
            }

            CHECK(name == m_Queue.front(), "Incorrect player actions sequence", name, m_Queue.front());
            m_Queue.pop_front();

            const unsigned playersInPot = GetPlayersInPot();
            Player::Queue activePlayers;
            GetActivePlayers(activePlayers);

            LOG_TRACE("Detecting round finish: in pot: [%s], queue: [%s], phase: [%s], active: [%s]") % playersInPot % m_Queue % m_Phase % activePlayers;
            if (playersInPot < 2 || m_Queue.empty() && (m_Phase == Phase::River || activePlayers.size() < 2))
                m_IsRoundFinished = true;

            if (m_State == State::Server && m_IsRoundFinished)
                return;

            if (m_Queue.empty() && !m_IsRoundFinished)
                SetPhase(static_cast<Phase::Value>(m_Phase + 1)), phaseChanged = true;
        }

        LOG_TRACE("Detect next player: finished: [%s], state: [%s], queue: [%s], action: [%s]") % m_IsRoundFinished % m_State % m_Queue % action;
        if (!m_IsRoundFinished && m_State == State::InitedClient && !m_Queue.empty() && action != Action::SmallBlind)
        {
            const std::string& botName = Player::ThisPlayer().Name();
            CHECK(!botName.empty(), "Bot name not inited, can't get next");
            if (m_Queue.front() == botName)
            {
                LOG_TRACE("Next player is bot, cards: [%s]") % GetPlayer(botName).Cards().size();
                if (phaseChanged || GetPlayer(botName).Cards().empty()) // workaround, cards may be received after decision needed
                    m_IsNeedDecision = true;
                else
                    SendRequest(false);
            }
        }
    }
    catch (CExcept& e)
    {
        LOG_ERROR("Failed to push action: [%s], name: [%s], amount: [%s], error: [%s]") % name % Action::ToString(action) % amount % e.what();
        APPEND_ARGS(e, name, action, amount);
        m_State = State::Uninited;
        throw;
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
    LOG_TRACE("Player: [%s], cards: [%s]") % name % cards;

    if (!m_Players.count(name))
        m_Players[name] = pcmn::Player(name, cfg::DEFAULT_STACK);

    GetPlayer(name).Cards(cards);

    // workaround, cards may be received after decision needed
    if (Player::ThisPlayer().Name() == name && m_IsNeedDecision)
    {
        SendRequest(false);
        m_IsNeedDecision = false;
    }
}

void TableLogic::ParseFlopCards(const net::Packet& packet)
{
    SCOPED_LOG(m_Log);

    for (int i = 0 ; i < packet.info().cards_size(); ++i)
    {
        m_Context.m_Data.m_Flop.push_back(packet.info().cards(i));
        m_Flop.push_back(Card().FromEvalFormat(packet.info().cards(i)));
    }

    if (m_Flop.size() >= 3)
    {
        for (unsigned i = 3; i <= m_Flop.size(); ++i)
        {
            pcmn::Card::List board(m_Flop.begin(), m_Flop.begin() + i);

            pcmn::Board parser(board);
            parser.Parse();
            m_Context.m_Data.m_Board.push_back(parser.GetValue());
        }
    }
}

void TableLogic::ParsePlayers(const net::Packet& packet)
{
    SCOPED_LOG(m_Log);

    for (int i = 0 ; i < packet.info().players_size(); ++i)
    {
        const net::Packet::Player& player = packet.info().players(i);

        pcmn::TableContext::Data::Player p;
        p.m_Name = player.name();

        m_Sequence.push_back(player.name());
        const PlayersMap::_Pairib inserted = m_Players.insert(std::make_pair(player.name(), Player(player.name(), player.stack())));
        CHECK(inserted.second, "Player name must be unique", player.name());

        if (player.cards_size() == 2)
        {
            pcmn::TableContext::Data::Hand hand;
            hand.m_Cards.push_back(player.cards(0));
            hand.m_Cards.push_back(player.cards(1));
            hand.m_PlayerIndex = i;

            m_Context.m_Data.m_Hands.push_back(hand);

            p.m_Percents = GetPlayerEquities(player.cards(0), player.cards(1), packet, m_Context);
            inserted.first->second.Cards(boost::assign::list_of(Card().FromEvalFormat(player.cards(0)))(Card().FromEvalFormat(player.cards(1))));
            
            Hand parser;
            if (!inserted.first->second.Cards().empty())
            {
                pcmn::Hand hand;
                static const pcmn::Card::List empty;
                hand.Parse(inserted.first->second.Cards(), empty);

                inserted.first->second.PushHand(hand.GetValue());

                if (m_Flop.size() >= 3)
                {
                    for (unsigned i = 3; i <= m_Flop.size(); ++i)
                    {
                        const pcmn::Card::List board(m_Flop.begin(), m_Flop.begin() + i);
                        hand.Parse(inserted.first->second.Cards(), board);
                        inserted.first->second.PushHand(hand.GetValue());
                    }
                }
            }

            for (const float eq : p.m_Percents)
                inserted.first->second.PushEquity(eq);
        }

        m_Context.m_Data.m_Players.push_back(p);
    }

    m_Button = m_Context.m_Data.m_Players.at(packet.info().button()).m_Name;
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
    std::copy(players.begin(), players.end(), std::back_inserter(m_NextRoundData));

    if (players.size() == 1 && players.front().Name() == Player::ThisPlayer().Name() && players.front().Cards() != Player::ThisPlayer().Cards())
    {
        // this is new player cards, it means that this is new round
        // we must send statistics before
        if (m_IsRoundFinished)
        {
            SendRequest(true);
            ResetPhase();

            if (!m_Queue.empty())
            {
                PushAction(m_Queue.front(), pcmn::Action::SmallBlind, m_Context.m_BigBlind / 2);
                PushAction(m_Queue.front(), pcmn::Action::BigBlind, m_Context.m_BigBlind);
            }
        }
    }
}

void TableLogic::GetActivePlayers(Player::Queue& players)
{
    for (const std::string& player : m_Sequence)
    {
        const Player& current = GetPlayer(player);
        if (current.State() != Player::State::Folded && current.Stack())
            UniqueAdd(players, current);
    }
}

unsigned TableLogic::GetPlayersInPot()
{
    unsigned result = 0;
    for (const std::string& player : m_Sequence)
    {
        const Player& current = GetPlayer(player);
        if (current.State() != Player::State::Folded)
            ++result;
    }
    return result;
}

Player::Position::Value TableLogic::GetNextPlayerPosition()
{
    SCOPED_LOG(m_Log);

    const unsigned totalPlayers = m_Sequence.size();
    unsigned leftInQueue = m_Queue.size();

    if (m_Phase == Phase::Preflop && m_Sequence.size() > 3)
    {
        if (leftInQueue <= 2)
            leftInQueue = 0;
        else
            leftInQueue -= 2; // don't calculate blinds
    }

    pcmn::Player::Position::Value result = pcmn::Player::Position::Middle;

    float step = static_cast<float>(m_Sequence.size()) / 3;
    if (step < 1)
        step = 1;

    if (leftInQueue <= step)
        result = pcmn::Player::Position::Later;
    else
    if (leftInQueue >= step * 2.5f)
        result = pcmn::Player::Position::Early;

    LOG_TRACE("Sequence: [%s], queue: [%s], step: [%s], result: [%s]") % m_Sequence % m_Queue % step % result;
    return result;
}

void TableLogic::ResetData()
{
    for (unsigned i = 0 ; i <= Phase::River; ++i)
        m_Actions[i].clear();

    for (PlayersMap::value_type& player : m_Players)
        player.second.Reset();

    m_Phase = Phase::Preflop;
    m_NextRoundData.clear();
    m_Context.m_Pot = 0;
    m_Flop.clear();
    m_NextRoundData.clear();
    m_IsRoundFinished = false;
}

bool TableLogic::FindExistingSmallBlind(const std::string& name, unsigned amount)
{
    for (unsigned i = 0; i < m_Actions[Phase::Preflop].size(); ++i)
    {
        ActionDesc& desc = m_Actions[Phase::Preflop][i];
        if (desc.m_Value == Action::SmallBlind)
        {
            if (desc.m_Amount < amount)
            {
                // invalid small blind amount, correct small and big blinds
                unsigned diff = amount - desc.m_Amount;

                desc.m_Amount = amount;
                m_Actions[Phase::Preflop][i + 1].m_Amount = amount * 2; 
                m_Context.m_BigBlind = amount * 2;

                {
                    Player& smallBlind = GetPlayer(name);
                    smallBlind.Bet(smallBlind.Bet() + diff);
                    smallBlind.TotalBet(smallBlind.TotalBet() + diff);
                    m_Context.m_Pot += diff;
                    if (m_State == State::Server)
                    {
                        assert(smallBlind.Stack() >= diff);
                        smallBlind.Stack(smallBlind.Stack() - diff);
                    }
                }

                diff *= 2;

                {
                    Player& bigBlind = GetPlayer(m_Actions[Phase::Preflop][i + 1].m_Name);
                    bigBlind.Bet(bigBlind.Bet() + diff);
                    bigBlind.TotalBet(bigBlind.TotalBet() + diff);
                    m_Context.m_Pot += diff;
                    if (m_State == State::Server)
                    {
                        assert(bigBlind.Stack() >= diff);
                        bigBlind.Stack(bigBlind.Stack() - diff);
                    }
                }

            }
            return true;
        }
    }
    return false;
}

void TableLogic::ParseSmallBlind(const net::Packet& packet)
{
    auto it = std::find_if
    (
        packet.phases(0).actions().begin(), 
        packet.phases(0).actions().end(),
        [](const net::Packet::Action& actionDsc) { return actionDsc.id() == Action::SmallBlind; }
    );

    m_Context.m_BigBlind = 0;
    if (it == packet.phases(0).actions().end())
    {
        it = std::find_if
        (
            packet.phases(0).actions().begin(), 
            packet.phases(0).actions().end(),
            [](const net::Packet::Action& actionDsc) { return actionDsc.id() == Action::Call; }
        );

        m_Context.m_BigBlind = (it == packet.phases(0).actions().end() ? 10 : it->amount()) * 2;
    }
    else
        m_Context.m_BigBlind = it->amount() * 2;
}

void TableLogic::ParsePlayerLoose(Player& current, BetSize::Value lastBigBet, Action::Value action)
{
    if (!current.Equities().empty() || current.GetActions().empty())
        return;
   
    if (lastBigBet <= BetSize::Normal && action == pcmn::Action::Fold)
    {
        // player folded, assuming that he has bad equities in case when bet is low
        float eq = 0;
        switch (lastBigBet)
        {
        case BetSize::VeryLow: eq = 5.0f; break;
        case BetSize::Low: eq = 15.0f; break;
        case BetSize::Normal: eq = 20.0f; break;
        }
        for (Phase::Value i = Phase::Preflop ; i <= Phase::River; i = static_cast<Phase::Value>(i + 1))
            current.PushEquity(eq);
    }
    else
    if (action == pcmn::Action::Loose)
    {
        Player::Queue activePlayers;
        GetActivePlayers(activePlayers);

        activePlayers.erase
        (
            std::remove_if
            (
                activePlayers.begin(), 
                activePlayers.end(), 
                [](const Player& lhs)
                {
                    return lhs.Equities().empty();
                }
            ),
            activePlayers.end()
        );

        // find player with smallest equity
        std::sort
        (
            activePlayers.begin(), 
            activePlayers.end(), 
            [](const Player& lhs, const Player& rhs)
            {
                return lhs.Equities().back() < rhs.Equities().back();
            }
        );

        float ratio = 0.1f;
        switch (lastBigBet)
        {
        case BetSize::VeryLow: ratio = 0.3f; break;
        case BetSize::Low: ratio = 0.4f; break;
        case BetSize::Normal: ratio = 0.6f; break;
        case BetSize::High: ratio = 0.8f; break;
        case BetSize::Huge: ratio = 1.0f; break;
        }
        float smallestEquity = activePlayers.empty() ? 20.0f : (activePlayers.front().Equities().back() * ratio);
        if (smallestEquity > 50)
            return; // undefined equity, to much scattered
        if (smallestEquity < 10)
            smallestEquity = 10;

        const float step = smallestEquity / 3 / (Phase::River - Phase::Preflop);
        float val = smallestEquity * 2 / 3;
        for (Phase::Value i = Phase::Preflop ; i <= Phase::River; i = static_cast<Phase::Value>(i + 1), val += step)
            current.PushEquity(val);    
    }
    
}

void TableLogic::Parse(const net::Packet& packet)
{
    TRY 
    {
        m_State = State::Server;
        m_Context = TableContext();

        // get small blind amount
        ParseSmallBlind(packet);

        // parse players and flop
        ParseFlopCards(packet);
        ParsePlayers(packet); 
        m_Context.m_Pot = 0;

        // simulate logic
        for (Phase::Value phase = Phase::Preflop ; phase < packet.phases_size(); phase = static_cast<Phase::Value>(phase + 1))
        {
            SetPhase(phase);

            BetSize::Value lastBigBet = BetSize::VeryLow;
            m_Context.m_MaxBet = 0;
            pcmn::Action::Value lastAction = phase != Phase::Preflop ? pcmn::Action::Unknown : pcmn::Action::BigBlind;
            pcmn::BetSize::Value lastAmount = phase != Phase::Preflop ? pcmn::BetSize::VeryLow : BetSize::Low;

            for (int i = 0; i < packet.phases(phase).actions_size(); ++i)
            {
                m_Context.m_Street = phase;
                const Action::Value action = static_cast<Action::Value>(packet.phases(phase).actions(i).id());
                const std::size_t playerIndex = static_cast<std::size_t>(packet.phases(phase).actions(i).player());
                const std::string& player = m_Context.m_Data.m_Players.at(playerIndex).m_Name;
                const unsigned amount = static_cast<unsigned>(packet.phases(phase).actions(i).amount());

                Player& current = GetPlayer(player);

                const Player::Position::Value position = GetNextPlayerPosition();
                LOG_TRACE("Position for: [%s] is: [%s]") % player % position;
                const BetSize::Value betValue = BetSize::FromAction(amount, m_Context.m_Pot, current.Stack(), m_Context.m_BigBlind);

                // skip useless actions
                const bool isUseful = Action::IsUseful(action);
                if (isUseful)
                {
                    if (m_Context.m_MaxBet < amount)
                        m_Context.m_MaxBet = amount;
                    
                    if (betValue > lastBigBet)
                        lastBigBet = betValue;

                    PushAction(player, action, amount);

                    TableContext::Data::Action resultAction;
                    resultAction.m_Action = action;
                    resultAction.m_PlayerIndex = playerIndex;
                    resultAction.m_Street = phase;
                    resultAction.m_Bet = static_cast<int>(betValue);
                    resultAction.m_Position = static_cast<int>(position);

                    m_Context.m_Data.m_Actions.push_back(resultAction);
                }

                ParsePlayerLoose(current, lastBigBet, action);

                if (isUseful)
                {
                    current.PushAction(phase, action, betValue, position, lastAction, lastAmount);
                    if ((action == pcmn::Action::Bet || action == pcmn::Action::Raise) && (action > lastAction || betValue > lastAmount))
                    {
                        lastAction = action;
                        lastAmount = betValue;
                    }
                }
            }
        }

        unsigned activePlayers = 0;
        Player::Queue playersInPot;
        for (const std::string& player : m_Sequence)
        {
            const Player& current = GetPlayer(player);
            if (current.State() != Player::State::Folded)
            {
                if (current.Stack())
                    ++activePlayers;
                UniqueAdd(playersInPot, current);
            }
        }

        if (!m_Queue.empty() && (activePlayers > 1 || activePlayers && playersInPot.size() > 1))
        {
            // need a decision
            const Player& current = GetPlayer(m_Queue.front());
            assert(current.Name() == "CLRN");
            const Player::Position::Value position = GetNextPlayerPosition();
            LOG_TRACE("Position for decision: [%s] is: [%s]") % current.Name() % position;

            for (const pcmn::Player& player : playersInPot)
                m_Context.m_Data.m_PlayersData.push_back(GetPlayer(player.Name()));

            m_Callback.MakeDecision(current, playersInPot, m_Context, position);
        }
        else
        {
            for (const std::string& player : m_Sequence)
                m_Context.m_Data.m_PlayersData.push_back(GetPlayer(player));

            // write statistics here
            m_Callback.WriteStatistics(m_Context.m_Data);
        }
    }
    CATCH_PASS_EXCEPTIONS("Failed to parse packet", packet.DebugString())
}

void TableLogic::SendRequest(const bool statistics)
{
    TRY 
    {
	    if (m_State == State::Server)
            return;

        CHECK(statistics || m_Phase == Phase::Preflop && m_Flop.empty() || m_Phase == Phase::Flop && m_Flop.size() >= 3 || m_Phase == Phase::Turn && m_Flop.size() >= 4 || m_Phase == Phase::River && m_Flop.size() == 5, "Invalid cards size of phase", m_Flop.size(), m_Phase);
	
	    PlayerQueue playerNames;
	    if (m_State == State::InitedClient)
	        playerNames = m_Sequence;
	    else
	    {
	        for (std::size_t i = 0 ; i < m_Actions[Phase::Preflop].size(); ++i)    
	        {
	            const ActionDesc& action = m_Actions[Phase::Preflop][i];
	            UniqueAdd(playerNames, action.m_Name);
                GetPlayer(action.m_Name); // insert player if not exists
	        }
	
	        const std::string& botName = Player::ThisPlayer().Name();
	        CHECK(!botName.empty());
	        UniqueAdd(playerNames, botName);
	        m_Button = playerNames.back();
	    }

        LOG_TRACE("Preparing server request, stats: [%s], state: [%s], players: [%s], button: [%s]") % statistics % m_State % playerNames % m_Button;
	
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
	
        bool anyUsefullActionsFound = false;
	    for (unsigned i = 0 ; i <= Phase::River; ++i)
	    {
	        const ActionDesc::List& actions = m_Actions[i];
	        if (actions.empty())
	            break;
	
	        net::Packet::Phase& phase = *packet.add_phases();
	        for (const ActionDesc& dsc : actions)
	        {
                if (!anyUsefullActionsFound && Action::IsUseful(dsc.m_Value))
                    anyUsefullActionsFound = true;

	            net::Packet::Action& action = *phase.add_actions();
	            action.set_amount(dsc.m_Amount);
	            action.set_id(dsc.m_Value);
	            action.set_player(players[dsc.m_Name]);
	        }
	    }

        if (anyUsefullActionsFound)
            m_Callback.SendRequest(packet, statistics);
    }
    CATCH_PASS_EXCEPTIONS("Failed to send request by table logic")
}

void TableLogic::SetPhase(const Phase::Value phase)
{
    m_Phase = phase;
    m_Queue.clear();
    m_IsNeedDecision = false;
    m_Context.m_MaxBet = 0;

    if (!ParseActionsIfNeeded())
        return;

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
        const unsigned smallBlindValue = State::Server == m_State ? m_Context.m_BigBlind / 2 : 0;

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

    LOG_TRACE("Phase set to: [%s], queue: [%s], state: [%s]") % m_Phase % m_Queue % m_State;

    if (m_State == State::InitedClient && !m_Queue.empty() && m_Phase != Phase::Preflop)
    {
        // ignore on preflop
        const std::string& botName = Player::ThisPlayer().Name();
        CHECK(!botName.empty(), "Bot name not inited, can't get next");
        if (m_Queue.front() == botName)
            m_IsNeedDecision = true;
    }

    if (m_State == State::InitedClient && m_Queue.empty()) // there is no active players
        m_IsRoundFinished = true;
}

Player& TableLogic::GetPlayer(const std::string& name)
{
    if (!m_Players.count(name))
        m_Players.insert(std::make_pair(name, Player(name, cfg::DEFAULT_STACK)));
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


void TableLogic::ResetPhase()
{
    if (!ParseActionsIfNeeded())
    {
        ResetData();
        return; // not complete data
    }

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

        if (m_Flop.size() != 5)
            continue;

        // workaround for case when player not showed cards
        if (currentPlayer.Cards().size() != 2 && !currentPlayer.Stack())
        {
            const Hand hand = {player, 0};
            hands.push_back(hand);
            continue;
        }
        else
        if (currentPlayer.Cards().size() != 2)
            continue;

        Card::List cards = m_Flop;
        std::copy(currentPlayer.Cards().begin(), currentPlayer.Cards().end(), std::back_inserter(cards));
       
        const short rank = m_Evaluator.GetRank(cards);

        const Hand hand = {player, rank};
        hands.push_back(hand);
    }
    
    if (allInPlayers.size() == 1 && hands.empty())
    {
        // workaround in case when all players folds we must set player stack back
        Player& currentPlayer = GetPlayer(allInPlayers.front());
        if (!currentPlayer.Stack())
            currentPlayer.Stack(currentPlayer.TotalBet());
    }

    bool isButtonMoved = false;
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

            // find all players with better hand
            unsigned maxBet = 0;
            for (const Hand& h : hands)
            {
                if (h.m_Rank <= hand.m_Rank)
                    continue;
                const unsigned bet = GetPlayer(h.m_Player).TotalBet();
                if (bet > maxBet)
                    maxBet = bet;
            }

            if (maxBet < GetPlayer(hand.m_Player).TotalBet())
                skip = true; // loose not all money

            // workaround, all in players may win without info about stacks, so they haven't stack and will be assumed as all in again
            if (skip)
            {
                if (!GetPlayer(hand.m_Player).Stack())
                    GetPlayer(hand.m_Player).Stack(m_Context.m_BigBlind + 1);
                continue;
            }

            // looser
            if (m_Button == hand.m_Player)
            {
                m_Button = GetNextPlayerName(m_Button);
                isButtonMoved = true;
            }

            RemovePlayer(hand.m_Player);
        }
    }

    if (!isButtonMoved && m_IsRoundFinished)
        m_Button = GetNextPlayerName(m_Button);

    // back up next round data
    Player::List nextRoundData = m_NextRoundData;

    // reset all data
    ResetData();

    // set data from this round
    if (nextRoundData.size() > 2)
    {
        Player::List tmp(nextRoundData.end() - 2, nextRoundData.end());
        nextRoundData.swap(tmp); // only data from blinds
    }

    for (const pcmn::Player& player : nextRoundData)
    {
        if (!player.Cards().empty())
            SetPlayerCards(player.Name(), player.Cards());
        else
        if (std::find(m_Sequence.begin(), m_Sequence.end(), player.Name()) != m_Sequence.end())
        {
             //Player& currentPlayer = GetPlayer(player.Name());
             //if (currentPlayer.Stack() && !player.Stack()) // do not set stacks at zero if players is still alive
             //    continue;

             SetPlayerStack(player.Name(), player.Stack());
        }
    }

    SetPhase(Phase::Preflop);
}

bool TableLogic::ParseActionsIfNeeded()
{
    if (m_State != State::Uninited)
        return true;

    const auto it = std::find_if
    (
        m_Actions[Phase::Preflop].begin(), 
        m_Actions[Phase::Preflop].end(),
        [](const ActionDesc& actionDsc) { return actionDsc.m_Value == Action::SmallBlind || actionDsc.m_Value == Action::Ante; }
    );

    if (it == m_Actions[Phase::Preflop].end())
        return false; // not complete data

    m_Queue.clear();
    m_Sequence.clear();

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
    {
        m_Button = m_Sequence.front(); // parsed by ante, without blinds
        m_IsRoundFinished = false; // round is not finished yet
    }

    m_State = State::InitedClient;
    return true;
}

void TableLogic::SetFlopCards(const Card::List& cards)
{
    CHECK(cards.size() >= 3 && cards.size() <= 5, cards.size());
    m_Flop = cards;
    if (m_IsNeedDecision)
    {
        SendRequest(false);
        m_IsNeedDecision = false;
    }
}

void TableLogic::RemovePlayer(const std::string& name)
{
    m_Players.erase(name);
    m_Sequence.erase(std::remove(m_Sequence.begin(), m_Sequence.end(), name), m_Sequence.end());
}



}