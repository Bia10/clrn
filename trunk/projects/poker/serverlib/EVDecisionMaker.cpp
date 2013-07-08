#include "EVDecisionMaker.h"
#include "Modules.h"

// полное мат. ожидание - сумма мат ожиданий по каждому игроку
// мат ожидание по игроку - сумма всех исходов 
// для каждой возможной ставки считаем подное мат. ожидание, берем максимальное значение

// мат. ожидание для bet:
// для каждого игрока мат. ожидание это сумма мат. ожиданий для всех его предполагаемых рук умноженное на вероятность руки:
// M[player] = ∑ P(hand) * M[hand]

// мат. ожидание руки считаетя как:
// M[hand] = M[call] + M[fold] + M[raise]
// M[call] = ((банк + ставка бота + колл оппонента) * вероятность выигрыша бота + (- ставка бота) * вероятность проигрыша бота) * вероятность колла
// M[fold] = (банк / кол-во оппонентов) * вероятность фолда
// M[raise] = (расчитывается мат. ожидание колла бота для рейза High) * вероятность рейза

// мат. ожидание для call
// для каждой предполагаемой руки оппонента:
// M[player] = ∑ P(hand) * M[hand]
// M[hand] = (размер банка + размер ставки оппонента) * вероятность выигрыша бота + (- размер ставки оппонента) * вероятность проигрыша бота

namespace srv
{

const unsigned CURRENT_MODULE_ID = Modules::Server;

float EVDecisionMaker::Player::GetReaction(pcmn::BetSize::Value bet, pcmn::Action::Value action) const
{
    const IStatistics::PlayerInfo::BetsMap::const_iterator itBet = m_ReactionsOnBet.find(bet);
    if (m_ReactionsOnBet.end() == itBet)
        return 0.0f;

    const IStatistics::PlayerInfo::ActionsMap::const_iterator itAction = itBet->second.find(action);
    if (itBet->second.end() == itAction)
        return 0.0f;

    return itAction->second;
}


EVDecisionMaker::EVDecisionMaker(ILog& logger, const pcmn::Evaluator& evaluator, IStatistics& stats, net::IConnection& connection)
    : m_Log(logger)
    , m_Evaluator(evaluator)
    , m_Stat(stats)
    , m_Connection(connection)
    , m_BigBlind()
    , m_MaxBet()
{

}

void EVDecisionMaker::MakeDecision(const pcmn::Player& player, const pcmn::Player::Queue& activePlayers, const pcmn::TableContext& context, const pcmn::Player::Position::Value position)
{
    SCOPED_LOG(m_Log);

    m_MaxBet = context.m_MaxBet;

    m_Bot = player;
    CHECK(m_Bot.Cards().size() == 2, "Can't make decision without info about bot cards");

    m_BigBlind = context.m_BigBlind;
    CHECK(m_BigBlind, "Can't make decision without info about big blind size");

    m_Flop.clear();
    for (const int card : context.m_Data.m_Flop)
        m_Flop.push_back(pcmn::Card().FromEvalFormat(card));

    m_Opponents.clear();
    for (const pcmn::Player& opp : activePlayers)
    {
        if (opp.Name() == player.Name())
            continue;

        m_Opponents.push_back(opp);
    }

    CHECK(!m_Opponents.empty(), "Can't make decision without info about opponents");

    // prepare table info
    const Table table = { context.m_Pot, context.m_MaxBet };

    // prepare opponents
    const Player::List opponents = PrepareOpponents();

    // get best EV
    const BestEV result = CalculateBestEV(table, opponents);
    

    // send response here
}

void EVDecisionMaker::SendRequest(const net::Packet& /*packet*/, bool /*statistics*/)
{
    throw std::exception("The method or operation is not implemented.");
}

void EVDecisionMaker::WriteStatistics(pcmn::TableContext::Data& data)
{
    m_Stat.Write(data);
}

float EVDecisionMaker::GetWinRate(const pcmn::Hand::Value hand)
{
    SCOPED_LOG(m_Log);
    // TODO: organize cache
    return m_Evaluator.GetEquity(m_Bot.Cards().front().ToEvalFormat(), m_Bot.Cards().back().ToEvalFormat(), m_Flop, hand);
}

float EVDecisionMaker::CalculateCallEV(const Table& table, const Player& opp)
{
    SCOPED_LOG(m_Log);

    float result = 0;

    // for each expected hand
    for (const HandsDetector::Result::value_type& handAndPercent : opp.m_Hands)
    {
        // calculate bot win percentage first
        const float winRate = GetWinRate(handAndPercent.first);

        // calculate EV for this hand
        const float ev = (table.m_Pot + table.m_Bet) * winRate - table.m_Bet * (1.0f - winRate);

        LOG_TRACE("Call EV for hand: [%s]:[%s] is: [%s], pot: [%s], bet: [%s]") 
            % handAndPercent.first 
            % handAndPercent.second
            % ev
            % table.m_Pot
            % table.m_Bet;

        // summarize
        result += ev * handAndPercent.second;
    }

    LOG_TRACE("Total call EV: [%s], pot: [%s], bet: [%s]") % result % table.m_Pot % table.m_Bet;
    return result;
}

float EVDecisionMaker::CalculateBetEV(const Table& tableBeforeBet, const Player& opp, pcmn::BetSize::Value bet)
{
    SCOPED_LOG(m_Log);

    float result = 0;

    const unsigned betSize = pcmn::BetSize::FromValue(bet, tableBeforeBet.m_Pot, m_Bot.Stack(), m_Bot.Bet(), m_BigBlind);

    const Table table = { tableBeforeBet.m_Pot, betSize };

    // for each expected hand
    for (const HandsDetector::Result::value_type& handAndPercent : opp.m_Hands)
    {
        // get chance of the call
        const float callChance = opp.GetReaction(bet, pcmn::Action::Call);

        // calculate EV of the call action
        const float callEV = CalculateCallEV(table, opp) * callChance;

        // get chance of the fold
        const float foldChance = opp.GetReaction(bet, pcmn::Action::Fold);

        // calculate EV of the fold action
        const float foldEV = (table.m_Pot * foldChance) / m_Opponents.size();

        // get chance of the raise
        const float raiseChance = opp.GetReaction(bet, pcmn::Action::Raise);

        // get average raise
        const unsigned expectedRaise = pcmn::BetSize::FromValue(pcmn::BetSize::High, table.m_Pot + betSize, opp.m_Player.Stack(), opp.m_Player.Bet(), m_BigBlind);

        // prepare table for this raise
        const Table raiseTable = { table.m_Pot + betSize, expectedRaise };

        // prepare opponents for this raise
        Player::List opps = PrepareOpponents();
        for (Player& current : opps)
        {
            if (current.m_Player.Name() != opp.m_Player.Name())
                continue;
        
            current.m_Player.PushAction
            (
                opp.m_Player.GetActions().size(), 
                pcmn::Action::Raise,
                pcmn::BetSize::High,
                pcmn::Player::Position::Later,
                m_MaxBet ? pcmn::Action::Raise : pcmn::Action::Bet,
                bet,
                pcmn::Player::Count::FromValue(m_Opponents.size())
            );
        }

        // calculate best EV for this raise
        const BestEV bestRaiseEV = CalculateBestEV(raiseTable, opps);

        // calculate raise EV
        const float raiseEV = bestRaiseEV.m_Value * raiseChance;

        LOG_TRACE("Bet EV for hand: [%s]:[%s], pot: [%s], bet: [%s], call: [%s], fold: [%s], raise: [%s], value: [%s], action: [%s]") 
            % handAndPercent.first 
            % handAndPercent.second
            % table.m_Pot
            % table.m_Bet
            % callEV
            % foldEV
            % raiseEV
            % expectedRaise
            % pcmn::Action::ToString(bestRaiseEV.m_Action);

        result += (callEV + foldEV + raiseEV);
    }

    return result;
}

EVDecisionMaker::BestEV EVDecisionMaker::CalculateBestEV(const Table& table, const Player::List& opps)
{
    BestEV result = {};

    // for each player
    // fetch statistics
    // calculate EV for all actions and bet sizes
    // return best value

    return std::move(result);
}

EVDecisionMaker::Player::List EVDecisionMaker::PrepareOpponents()
{
    EVDecisionMaker::Player::List result;

    // fetch statistics here

    return std::move(result);
}

} // namespace srv