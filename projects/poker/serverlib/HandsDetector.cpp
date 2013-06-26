#include "HandsDetector.h"
#include "Modules.h"
#include "Evaluator.h"
#include "Board.h"

namespace srv
{

const unsigned CURRENT_MODULE_ID = Modules::Server;
const unsigned POCKET_HAND_MASK = pcmn::Hand::Suited | pcmn::Hand::Connectors | pcmn::Hand::OneHigh | pcmn::Hand::BothHigh | pcmn::Hand::Ace | pcmn::Hand::PoketPair;
const unsigned FLOP_HAND_MASK = pcmn::Hand::HighCard | pcmn::Hand::Pair | pcmn::Hand::TwoPairs | pcmn::Hand::ThreeOfKind | pcmn::Hand::Straight | pcmn::Hand::Flush | pcmn::Hand::FullHouse | pcmn::Hand::FourOfKind | pcmn::Hand::StraightFlush;
const unsigned KICKERS_MASK = pcmn::Hand::LowKicker | pcmn::Hand::GoodKicker | pcmn::Hand::TopKicker;

HandsDetector::HandsDetector(ILog& log, IStatistics& stats)
    : m_Log(log)
    , m_Statistic(stats)
{

}

template<typename Arg>
void NormalizeByMask(Arg& map, unsigned mask)
{
    Arg temp;
    temp.swap(map);

    for (const auto& pair : temp)
    {
        // remove characteristics
        const pcmn::Hand::Value possibleHandAfterFlop = static_cast<pcmn::Hand::Value>(pair.first & mask);
        map[possibleHandAfterFlop] += pair.second;
    }

    // find summ
    float summ = 0;
    for (const auto& pair : map)
        summ += pair.second;

    // normalize coefficient 
    const float coefficient = 1.0f / summ;

    for (auto& pair : map)
        pair.second *= coefficient;
}

template<typename Arg, typename Func>
void NormalizeByFunctor(Arg& map, Func func)
{
    Arg temp;
    temp.swap(map);

    for (const auto& pair : temp)
    {
        // remove characteristics
        const pcmn::Hand::Value possibleHandAfterFlop = static_cast<pcmn::Hand::Value>(func(pair.first));
        map[possibleHandAfterFlop] += pair.second;
    }
}

float HandsDetector::GetHandChance(pcmn::Hand::Value preflopHand, const pcmn::Card::List& board, pcmn::Hand::Value possibleHand)
{
    // get all possible cards for this preflop hand description
    const pcmn::Board::HandsList& hands = pcmn::Board().GetCardsByHand(preflopHand);

    // for each preflop hand parse board cards and compare result with possible hand
    unsigned count = 0;
    for (const pcmn::Card::List& cards : hands)
    {
        // check for dead cards in the player hand
        if (std::find_if(board.begin(), board.end(), [&](const pcmn::Card& c){ return c == cards.front() || c == cards.back(); }) != board.end())
            continue;

        pcmn::Hand hand;
        hand.Parse(cards, board);

        if (hand.GetValue() & possibleHand)
            ++count; // this hand is possible
    }

    // calculate chance that player may have possible hand on this flop
    return static_cast<float>(count) / hands.size();
}

void HandsDetector::DetectHand(const pcmn::Card::List& board, const pcmn::Player& player, Result& result, unsigned totalPlayers)
{
    SCOPED_LOG(m_Log);

    TRY 
    {
        result.clear();

        CHECK(!player.GetActions().empty(), "Can't detect hand without actions");

        // player actions on all streets
        const pcmn::Player::Actions& streets = player.GetActions();

        for (unsigned street = 0; street < streets.size(); ++street)
        {
            const pcmn::Player::ActionDesc::List& actions = streets[street];

            // fetch possible hands for this street
            IStatistics::PlayerInfo playerInfo;
            playerInfo.m_Actions = actions;
            playerInfo.m_Name = player.Name();

            m_Statistic.GetHands(playerInfo, street, totalPlayers);

            if (!street)
            {
                // just copy on preflop
                for (const IStatistics::PlayerInfo::Hands::value_type& pair : playerInfo.m_Hands)
                {
                    LOG_TRACE("Preflop hand for player: [%s] is: [%s]:[%s]") % player.Name() % pair.first % pair.second;
                    result.insert(pair);
                }
                continue;
            }

            // remove characteristics from preflop
            NormalizeByFunctor(playerInfo.m_Hands, [](pcmn::Hand::Value hand) 
            { 
                // remove preflop characteristics
                hand = static_cast<pcmn::Hand::Value>(hand & ~POCKET_HAND_MASK);

                // if hand is big remove kickers and power
                const pcmn::Hand::Value onlyReadyHands = static_cast<pcmn::Hand::Value>(hand & FLOP_HAND_MASK);
                if (onlyReadyHands > pcmn::Hand::Flush || onlyReadyHands == pcmn::Hand::ThreeOfKind)
                    return static_cast<unsigned>(onlyReadyHands);
                else
                if (onlyReadyHands >= pcmn::Hand::TwoPairs)
                    return hand & ~KICKERS_MASK;
                else
                    return static_cast<unsigned>(hand);
            });

            // for each hand in result we must detect chance that player made one of the hands from this street

            // copy results from the previous street
            Result lastStreetResults = result;
            result.clear();

            // remove flop characteristics, only preflop needed
            NormalizeByMask(lastStreetResults, POCKET_HAND_MASK);

            // enumerate all possible hands which player may have on this street
            for (const IStatistics::PlayerInfo::Hands::value_type& pair : playerInfo.m_Hands)
            {
                // remove preflop characteristics
                const pcmn::Hand::Value possibleHandAfterFlop = static_cast<pcmn::Hand::Value>(pair.first & ~POCKET_HAND_MASK);

                LOG_TRACE("Checking possible hand: [%s]") % pair.first;

                // enumerate all already calculated hands 
                for (const Result::value_type& calculatedHandPair : lastStreetResults)
                {
                    const pcmn::Hand::Value calculatedPreflopHand = calculatedHandPair.first;

                    LOG_TRACE("Comparing possible hand: [%s] with calculated: [%s]") % possibleHandAfterFlop % calculatedPreflopHand;

                    // detect chance that player have this hand on this flop
                    const float possibeHandChance = GetHandChance(calculatedHandPair.first, board, possibleHandAfterFlop);
                    const float chanceWithPrelop = possibeHandChance * calculatedHandPair.second * pair.second;

                    // add preflop characteristics of this hand to result hand
                    const pcmn::Hand::Value finalHand = static_cast<pcmn::Hand::Value>(possibleHandAfterFlop | calculatedPreflopHand); 

                    LOG_TRACE("Chance of the final hand: [%s] is: [%s], with preflop: [%s]") % finalHand % possibeHandChance % chanceWithPrelop;

                    // add this hand in final result
                    if (chanceWithPrelop)
                    {
                        if (result.count(finalHand))
                            result[finalHand] += chanceWithPrelop;
                        else
                            result[finalHand] = chanceWithPrelop;
                    }
                }
            }
        }

        // remove preflop characteristics
        NormalizeByMask(result, ~POCKET_HAND_MASK);
    }
    CATCH_PASS_EXCEPTIONS("Failed to detect hand", player.Name())
}

}
