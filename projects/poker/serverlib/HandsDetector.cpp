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

void HandsDetector::CalculateWeightsByPossibleHands(const HandsMap& hands, unsigned street)
{
    SCOPED_LOG(m_Log);

    HandDesc::List thisStreetWeights;
    if (street)
    {
        thisStreetWeights = m_PossibleCards;
        for (HandDesc& currentHand : thisStreetWeights)
            currentHand.m_Weight = 0;
    }

    pcmn::Hand parser;
    static const pcmn::Card::List empty;
    unsigned counter = 0;

    // calculate weights for this street
    HandDesc::List* target = street ? &thisStreetWeights : &m_PossibleCards; // calculate directly in member on preflop
    for (HandDesc& currentHand : *target)
    {
        parser.Parse(currentHand.m_Cards, street ? m_Board : empty);
        pcmn::Hand::Value parsed = parser.GetValue();
        if (street)
            parsed = static_cast<pcmn::Hand::Value>(parsed & (pcmn::Hand::FLOP_HAND_MASK | pcmn::Hand::POWER_MASK));
        if (street == 3) // remove draws from river
            parsed = static_cast<pcmn::Hand::Value>(parsed & ~pcmn::Hand::DRAWS_MASK);

        const HandsMap::const_iterator it = hands.find(parsed);
        if (it != hands.end())
            currentHand.m_Weight += it->second;
        else
            ++counter;
    }

    LOG_TRACE("Filtered [%s] hands from street [%s]") % counter % street;

    if (!street)
        return;

    // now multiply previous weights by this street weights
    assert(thisStreetWeights.size() == m_PossibleCards.size());

    for (std::size_t i = 0 ; i < m_PossibleCards.size(); ++i)
        m_PossibleCards[i].m_Weight *= (thisStreetWeights[i].m_Weight * street);
}

void HandsDetector::GetAllPossibleHands(Result& result, unsigned street)
{
    pcmn::Hand parser;
    static const pcmn::Card::List empty;
    float totalWeight = 0.0f;

    // remove without weight
    m_PossibleCards.erase(std::remove_if(m_PossibleCards.begin(), m_PossibleCards.end(), [](const HandDesc& h) { return !h.m_Weight; }), m_PossibleCards.end());

    if (street && m_PossibleCards.size() > 100)
    {
        // sort by weight
        std::sort(m_PossibleCards.begin(), m_PossibleCards.end(), [](const HandDesc& lhs, const HandDesc& rhs) { return lhs.m_Weight > rhs.m_Weight; });

        // remove lowest
        m_PossibleCards.resize(m_PossibleCards.size() / 5);
    }

    // parse all cards and save with weights
    for (HandDesc& currentHand : m_PossibleCards)
    {
        parser.Parse(currentHand.m_Cards, street ? m_Board : empty);
        pcmn::Hand::Value parsed = parser.GetValue();
        if (street)
            parsed = static_cast<pcmn::Hand::Value>(parsed & (pcmn::Hand::FLOP_HAND_MASK | pcmn::Hand::POWER_MASK));
        if (street == 3) // remove draws from river
            parsed = static_cast<pcmn::Hand::Value>(parsed & ~pcmn::Hand::DRAWS_MASK);

        result[parsed] += currentHand.m_Weight;
        totalWeight += currentHand.m_Weight;
    }

    // recalculate weights to percents
    for (Result::value_type& pair : result)
        pair.second = pair.second / totalWeight;
}

void HandsDetector::DetectHand(const pcmn::Card::List& boardInput, const pcmn::Player& player, Result& result, unsigned totalPlayers)
{
    SCOPED_LOG(m_Log);

    TRY 
    {
        result.clear();
        m_PossibleCards.clear();
        m_Board.clear();

        CHECK(!player.GetActions().empty(), "Can't detect hand without actions");

        // player actions on all streets
        const pcmn::Player::Actions& streets = player.GetActions();

        // all possible cards
        const pcmn::Board::HandsList allhands = pcmn::Board().GetAllPocketCards();

        // generate list of weights
        m_PossibleCards.reserve(allhands.size());
        for (const pcmn::Card::List& cards : allhands)
        {
            // check for dead cards in the player hand
            if (std::find_if(boardInput.begin(), boardInput.end(), [&](const pcmn::Card& c){ return c == cards.front() || c == cards.back(); }) != boardInput.end())
                continue;

            m_PossibleCards.push_back(HandDesc(cards));
        }

        for (unsigned street = 0; street < streets.size(); ++street)
        {
            m_Board = boardInput;
            switch (street)
            {
            case 0: m_Board.clear(); break;
            case 1: m_Board.resize(3); break;
            case 2: m_Board.resize(4); break;
            case 3: m_Board.resize(5); break;
            default: assert(false);
            }

            const pcmn::Player::ActionDesc::List& actions = streets[street];

            // fetch possible hands for this street
            IStatistics::PlayerInfo playerInfo;
            playerInfo.m_Actions = actions;
            playerInfo.m_Name = player.Name();

            m_Statistic.GetHands(playerInfo, street, totalPlayers);

            // generate list of hands
            HandsMap hands;

            for (const IStatistics::PlayerInfo::Hands::value_type& pair : playerInfo.m_Hands)
            {
                LOG_TRACE("Hand on street: [%s] for player: [%s] is: [%s]:[%s]") % street % player.Name() % pair.first % pair.second;
                hands.insert(pair);
            }

            // filter all possible cards by all possible hands
            CalculateWeightsByPossibleHands(hands, street);
        }

        // get all possible hands
        GetAllPossibleHands(result, streets.size() - 1);
    }
    CATCH_PASS_EXCEPTIONS("Failed to detect hand", player.Name())
}

}
