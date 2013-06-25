#include "HandsDetector.h"
#include "Modules.h"
#include "Evaluator.h"
#include "Board.h"

namespace srv
{

const unsigned CURRENT_MODULE_ID = Modules::Server;
const unsigned POCKET_HAND_MASK = pcmn::Hand::Suited | pcmn::Hand::Connectors | pcmn::Hand::OneHigh | pcmn::Hand::BothHigh | pcmn::Hand::Ace | pcmn::Hand::PoketPair;

HandsDetector::HandsDetector(ILog& log, IStatistics& stats)
    : m_Log(log)
    , m_Statistic(stats)
{

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

        if ((hand.GetValue() & possibleHand) == possibleHand)
            ++count; // this hand is possible
    }

    // calculate chance that player may have possible hand on this flop
    return static_cast<float>(count) / hands.size();
}

void HandsDetector::DetectHand(const pcmn::Card::List& board, const pcmn::Player& player, Result& result)
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
            IStatistics::PlayerInfo::List players(1);
            players.front().m_Actions = actions;
            players.front().m_Name = player.Name();

            m_Statistic.GetHands(players, street);

            if (!street)
            {
                // just copy on preflop
                for (const IStatistics::PlayerInfo::Hands::value_type& pair : players.front().m_Hands)
                {
                    LOG_TRACE("Preflop hand for player: [%s] is: [%s]:[%s]") % player.Name() % pair.first % pair.second;
                    result.insert(pair);
                }
                continue;
            }

            // for each hand in result we must detect chance that player made one of the hands from this street

            // copy results from the previous street
            Result lastStreetResults = result;
            result.clear();

            // enumerate all possible hands which player may have on this street
            for (const IStatistics::PlayerInfo::Hands::value_type& pair : players.front().m_Hands)
            {
                // remove preflop characteristics
                const pcmn::Hand::Value possibleHandAfterFlop = static_cast<pcmn::Hand::Value>(pair.first & ~POCKET_HAND_MASK);

                LOG_TRACE("Checking possible hand: [%s]") % pair.first;

                // enumerate all already calculated hands 
                for (const Result::value_type& calculatedHandPair : lastStreetResults)
                {
                    // remove after flop characteristics
                    const pcmn::Hand::Value calculatedPreflopHand = static_cast<pcmn::Hand::Value>(calculatedHandPair.first & POCKET_HAND_MASK);

                    LOG_TRACE("Comparing possible hand: [%s] with calculated: [%s]") % possibleHandAfterFlop % calculatedPreflopHand;

                    // detect chance that player have this hand on this flop
                    const float possibeHandChance = GetHandChance(calculatedHandPair.first, board, possibleHandAfterFlop);

                    // add preflop characteristics of this hand to result hand
                    const pcmn::Hand::Value finalHand = static_cast<pcmn::Hand::Value>(possibleHandAfterFlop | calculatedPreflopHand); 

                    LOG_TRACE("Chance of the final hand: [%s] is: [%s]") % finalHand % possibeHandChance;

                    // add this hand in final result
                    if (possibeHandChance)
                        assert(result.insert(std::make_pair(finalHand, possibeHandChance)).second);
                }
            }
        }
    }
    CATCH_PASS_EXCEPTIONS("Failed to detect hand", player.Name())
}

}
