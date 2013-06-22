#include "Board.h"
#include "Config.h"

#include <boost/assign/list_of.hpp>

namespace pcmn
{

Board::CachedHands Board::s_HandsCache;
boost::mutex Board::s_CacheMutex;
static const unsigned POCKET_HAND_MASK = Hand::Suited | Hand::Connectors | Hand::OneHigh | Hand::BothHigh | Hand::Ace | Hand::PoketPair;


Board::Board(const Card::List& cards /*= Card::List()*/) : m_Board(cards)
{

}

Board::HandsList Board::GetCardsByHand(const Hand::Value hand)
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

    return FilterCards(it->second, hand);
}

void Board::GeneratePossibleHands(HandsList& result, Hand::Value hand) const
{
    static const std::size_t maxSize = static_cast<std::size_t>(std::pow(cfg::CARD_DECK_SIZE, 2)) / 2;
    static const Card::List emptyBoard;

    // use only preflop hand description
    hand = static_cast<Hand::Value>(hand & POCKET_HAND_MASK);

    result.reserve(maxSize);

    Hand parser;

    for (short first = 0 ; first < cfg::CARD_DECK_SIZE; ++first)
    {
        for (short second = first + 1 ; second < cfg::CARD_DECK_SIZE; ++second)
        {
            assert(second != first);

            const Card::List possibleCards = boost::assign::list_of(Card().FromEvalFormat(first))(Card().FromEvalFormat(second));
            parser.Parse(possibleCards, emptyBoard);

            if ((parser.GetValue() & hand) == hand)
                result.push_back(possibleCards);
        }
    }
}

Board::HandsList Board::FilterCards(const HandsList& src, const Hand::Value hand)
{
    // can't filter without board cards or without any flop description
    if (m_Board.empty() || !(~POCKET_HAND_MASK & hand))
        return src;

    Hand parser;

    HandsList result;
    result.reserve(src.size());
    std::copy_if(src.begin(), src.end(), std::back_inserter(result), [&](const Card::List& cards) 
        {
            // check for dead cards in the player hand
            if (std::find_if(m_Board.begin(), m_Board.end(), [&](const Card& c){ return c == cards.front() || c == cards.back(); }) != m_Board.end())
                return false;

            parser.Parse(cards, m_Board);

            return (parser.GetValue() & hand) == hand;
        }
    );
    return result;
}

}

