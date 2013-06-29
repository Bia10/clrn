#include "HandsDetector.h"
#include "Modules.h"
#include "Evaluator.h"
#include "Board.h"

namespace srv
{

const unsigned CURRENT_MODULE_ID = Modules::Server;

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
        map[possibleHandAfterFlop].m_Rate += pair.second.m_Rate;
    }
}

float HandsDetector::GetFlopHandChanceByPreflopHand(pcmn::Hand::Value preflopHand, const pcmn::Card::List& board, pcmn::Hand::Value possibleHand)
{
    // get all possible cards for this preflop hand description
    const pcmn::Board::HandsList& hands = pcmn::Board(board).GetCardsByHand(preflopHand);

    // for each preflop hand parse board cards and compare result with possible hand
    unsigned count = 0;
    for (const pcmn::Card::List& cards : hands)
    {
        // check for dead cards in the player hand
        if (std::find_if(board.begin(), board.end(), [&](const pcmn::Card& c){ return c == cards.front() || c == cards.back(); }) != board.end())
            continue;

        pcmn::Hand hand;
        hand.Parse(cards, board);

        if ((hand.GetValue() & possibleHand) == possibleHand)
            ++count; // this hand is possible
    }

    // calculate chance that player may have possible hand on this flop
    return static_cast<float>(count) / hands.size();
}

float HandsDetector::GetPreflopHandChanceByFlopHand(pcmn::Hand::Value preflopHand, const pcmn::Card::List& board, pcmn::Hand::Value flopHand)
{
    // get all possible hands
    const pcmn::Board::HandsList& hands = pcmn::Board().GetAllPocketCards();

    // hands that matched flop hand and preflop hand
    unsigned count = 0;

    Cache::iterator it = m_Cache.find(flopHand);
    if (it == m_Cache.end())
    {
        it = m_Cache.insert(std::make_pair(flopHand, Hands())).first;

        for (const pcmn::Card::List& cards : hands)
        {
            // check for dead cards in the player hand
            if (std::find_if(board.begin(), board.end(), [&](const pcmn::Card& c){ return c == cards.front() || c == cards.back(); }) != board.end())
                continue;

            pcmn::Hand hand;
            hand.Parse(cards, board);

            if ((hand.GetValue() & flopHand) == flopHand)
                it->second.push_back(cards);
        }

    }

    for (const pcmn::Card::List& cards : it->second)
    {
        pcmn::Hand hand;
        hand.Parse(cards, board);

        if ((hand.GetValue() & preflopHand) == preflopHand)
            ++count; // this hand is matched with preflop hand     
    }

    return static_cast<float>(count) / it->second.size();
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
            m_Cache.clear();
            const pcmn::Player::ActionDesc::List& actions = streets[street];

            pcmn::Player::ActionDesc::List sortedActions(actions);
            std::sort(sortedActions.begin(), sortedActions.end());

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
                    LOG_TRACE("Preflop hand for player: [%s] is: [%s]:[%s]") % player.Name() % pair.first % pair.second.m_Rate;
                    result.insert(std::make_pair(pair.first, pair.second.m_Rate));
                }
                continue;
            }

            // remove characteristics from preflop
//             NormalizeByFunctor(playerInfo.m_Hands, [](pcmn::Hand::Value hand) 
//             { 
//                 // remove power and kickers characteristics
//                 if (hand & pcmn::Hand::POWER_MASK)
//                     return static_cast<pcmn::Hand::Value>(pcmn::Hand::Pair);
//                 else
//                     return static_cast<pcmn::Hand::Value>(hand & (pcmn::Hand::FLOP_HAND_MASK | pcmn::Hand::DRAWS_MASK));
// //                 // remove preflop characteristics
// //                 hand = static_cast<pcmn::Hand::Value>(hand & ~pcmn::Hand::POCKET_HAND_MASK);
// // 
// //                 // if hand is big remove kickers and power
// //                 const pcmn::Hand::Value onlyReadyHands = static_cast<pcmn::Hand::Value>(hand & pcmn::Hand::FLOP_HAND_MASK);
// //                 if (onlyReadyHands > pcmn::Hand::Flush || onlyReadyHands == pcmn::Hand::ThreeOfKind)
// //                     return static_cast<unsigned>(onlyReadyHands);
// //                 else
// //                 if (onlyReadyHands >= pcmn::Hand::TwoPairs)
// //                     return hand & ~pcmn::Hand::KICKERS_MASK;
// //                 else
// //                     return static_cast<unsigned>(hand);
//             });

            // for each hand in result we must detect chance that player made one of the hands from this street

            // copy results from the previous street
            Result lastStreetResults = result;
            result.clear();

            // remove flop characteristics, only preflop needed
            NormalizeByMask(lastStreetResults, pcmn::Hand::POCKET_HAND_MASK);

            // enumerate all possible hands which player may have on this street
            for (const IStatistics::PlayerInfo::Hands::value_type& flopHandPair : playerInfo.m_Hands)
            {
                // remove preflop characteristics
                const pcmn::Hand::Value flopHand = static_cast<pcmn::Hand::Value>(flopHandPair.first & ~pcmn::Hand::POCKET_HAND_MASK);

                LOG_TRACE("Checking possible hand: [%s]:[%s]") % flopHandPair.first % flopHandPair.second.m_Rate;

                // enumerate all already calculated hands 
                for (const Result::value_type& calculatedPreflopHandPair : lastStreetResults)
                {
                    const pcmn::Hand::Value calculatedPreflopHand = calculatedPreflopHandPair.first;

                    LOG_TRACE("Comparing possible hand: [%s] with calculated: [%s]") % flopHand % calculatedPreflopHand;

                    // detect chance that player have flop hand with this preflop hand
                    const float flopHandChance = GetFlopHandChanceByPreflopHand(calculatedPreflopHand, board, flopHand);

                    if (!flopHandChance)
                        continue;

                    // получаем вероятность того что имея расчитываемую руку игрок сделает именно такие действия
                    // т.е. вытаскиваем статистику по игроку именно с такой рукой
                    const IStatistics::PlayerInfo::PossibleActions::const_iterator it = flopHandPair.second.m_Actions.find(sortedActions.back());
                    assert(it != flopHandPair.second.m_Actions.end());
                    const float thisActionsWithHandChance = it->second;

                    // calculate by Bayes' theorem
                    // р(у него есть сет и он пушит)=р(он пушит с сетом) * р(у него есть сет)/ (р(он пушит с сетом)*р(у него есть сет)+р(он пушит без сета)*р(у него нет сета))
                    const float chanceOfThisFlopHand = 
                        (thisActionsWithHandChance * flopHandChance) // chance that he have this preflop hand with this flop hand * chance of this preflop hand with this actions
                        / 
                        (thisActionsWithHandChance * flopHandChance + (1.0f - flopHandPair.second.m_Rate) * (1.0f - flopHandChance));

                    // detect chance that player have preflop hand with this flop hand
                   // const float preflopHandChance = GetPreflopHandChanceByFlopHand(calculatedPreflopHandPair.first, board, flopHand);

                    // calculate by Bayes' theorem
//                     const float chanceOfFlopHand = 
//                         (preflopHandChance * calculatedPreflopHandPair.second) // chance that he have this preflop hand with this flop hand * chance of this preflop hand with this actions
//                         / 
//                         (preflopHandChance * calculatedPreflopHandPair.second + (1.0f - flopHandChance) * (1.0f - calculatedPreflopHandPair.second));

//                     const float chanceOfFlopHand = calculatedPreflopHandPair.second * flopHandChance;
//                     result[flopHand] += chanceOfFlopHand;
                    //assert(result.insert(std::make_pair(flopHand, chanceOfFlopHand)).second);

//                     const float chanceWithPrelop = possibeHandChance * calculatedHandPair.second * pair.second;
// 
//                     // add preflop characteristics of this hand to result hand
//                     const pcmn::Hand::Value finalHand = static_cast<pcmn::Hand::Value>(possibleHandAfterFlop | calculatedPreflopHand); 
// 
//                     LOG_TRACE("Chance of the final hand: [%s] is: [%s], with preflop: [%s]") % finalHand % possibeHandChance % chanceWithPrelop;
// 
//                     // add this hand in final result
//                     if (chanceWithPrelop)
//                     {
//                         if (result.count(finalHand))
//                             result[finalHand] += chanceWithPrelop;
//                         else
//                             result[finalHand] = chanceWithPrelop;
//                     }

                    const pcmn::Hand::Value finalHand = static_cast<pcmn::Hand::Value>(flopHand | calculatedPreflopHand); 

                    // add this hand in final result
                    if (result.count(finalHand))
                        result[finalHand] += chanceOfThisFlopHand;
                    else
                        result[finalHand] = chanceOfThisFlopHand;
                }
            }
        }

        // remove preflop characteristics
        NormalizeByMask(result, ~pcmn::Hand::POCKET_HAND_MASK);
    }
    CATCH_PASS_EXCEPTIONS("Failed to detect hand", player.Name())
}

}
