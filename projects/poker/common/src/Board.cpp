#include "Board.h"
#include "Config.h"

#include <boost/assign/list_of.hpp>

namespace pcmn
{

Board::CachedHands Board::s_HandsCache;
boost::mutex Board::s_CacheMutex;

Board::Board(const Card::List& cards /*= Card::List()*/) : m_Board(cards)
{

}

// TODO: подумать как кэшировать значения для готовых рук и доски.
// сейчас это не работает т.к. закешировав пару для конкретной доски мы будем ее отдавать для других досок
// один из вариантов - получать кэшированные руки для префлопа и уже их фильтровать по готовоым рукам и флопу

const Board::HandsList& Board::GetCardsByHand(const Hand::Value hand)
{
    boost::unique_lock<boost::mutex> lock(s_CacheMutex);

    CachedHands::const_iterator it = s_HandsCache.find(hand);
    if (it == s_HandsCache.end())
    {
        lock.unlock();

        // not found, generate new
        HandsList generated;
        GeneratePossibleHands(generated, hand);

        lock.lock();
        it = s_HandsCache.insert(it, std::make_pair(hand, generated));
    }

    return it->second;
}

void Board::GeneratePossibleHands(HandsList& result, const Hand::Value hand) const
{
    bool dead[cfg::CARD_DECK_SIZE] = {false};
    static const std::size_t maxSize = static_cast<std::size_t>(std::pow(cfg::CARD_DECK_SIZE, 2)) / 2;

    for (const Card& card : m_Board)
        dead[card.ToEvalFormat()] = true;

    result.reserve(maxSize);

    Hand parser;

    for (short first = 0 ; first < cfg::CARD_DECK_SIZE; ++first)
    {
        for (short second = first + 1 ; second < cfg::CARD_DECK_SIZE; ++second)
        {
            if (second == first || dead[first] || dead[second])
                continue;

            Card::List possibleCards = boost::assign::list_of(Card().FromEvalFormat(first))(Card().FromEvalFormat(second));
            parser.Parse(possibleCards, m_Board);

            if ((parser.GetValue() & hand) == hand)
                result.push_back(possibleCards);
        }
    }
}

}

