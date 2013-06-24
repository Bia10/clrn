#include "Board.h"
#include "Config.h"

#include <boost/assign/list_of.hpp>

namespace pcmn
{

Board::CachedHands Board::s_HandsCache;
boost::mutex Board::s_CacheMutex;
static const unsigned POCKET_HAND_MASK = Hand::Suited | Hand::Connectors | Hand::OneHigh | Hand::BothHigh | Hand::Ace | Hand::PoketPair;


Board::Board(const Card::List& cards /*= Card::List()*/) : m_Board(cards), m_Value(Unknown)
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

void Board::Parse()
{
    m_Value = Unknown;

    if (m_Board.empty())
        return; // nothing to do

    assert(m_Board.size() >= 3 && m_Board.size() <= 5);

    // sort board by card value
    std::sort(m_Board.begin(), m_Board.end(), [](const Card& lhs, const Card& rhs) { return lhs.m_Value < rhs.m_Value; });

    if (m_Board.back().m_Value < Card::Jack)
        m_Value = static_cast<Value>(m_Value | Low);

    const unsigned highCount = std::count_if(m_Board.begin(), m_Board.end(), [](const Card& c) { return c.m_Value > Card::Ten; });
    if (highCount > m_Board.size() / 2)
        m_Value = static_cast<Value>(m_Value | High);

    if (m_Board.back().m_Value == Card::Ace)
        m_Value = static_cast<Value>(m_Value | Ace);

    std::vector<int> board; // integers because ace may on the beginning of the sequence
    if (m_Value & Ace)
        board.push_back(Card::Two - 1); // ace

    for (const Card& c : m_Board)
    {
        if (std::find(board.begin(), board.end(), c.m_Value) == board.end())
            board.push_back(c.m_Value);
    }

    if (board.size() >= 3)
    {
        // check for straight
        for (unsigned i = 0 ; i < board.size() - 3; ++i)
        {
            // maximum difference 2, p, 3, 5, 6 = 4
            // minimum difference 2, 3, 4, 6, p = 3

            const int from = board[i];
            const int to = board[i + 3];
            const int diff = to - from;
            if (diff <= 5 && diff >= 3)
            {
                m_Value = static_cast<Value>(m_Value | Straight);
                break;
            }
        }

        // check for straight draw
        if (!(m_Value & Straight))
        {
            for (unsigned i = 0 ; i < board.size() - 2; ++i)
            {
                // maximum difference 2, p, p, 5, 6 = 4
                // minimum difference 2, 3, 4, p, p = 2

                const int from = board[i];
                const int to = board[i + 2];
                const int diff = to - from;
                if (diff <= 4 && diff >= 2)
                {
                    m_Value = static_cast<Value>(m_Value | StraightDraw);
                    break;
                }
            }
        }
    }

    // check for flush
    std::map<Suit::Value, unsigned> counters;
    for (const Card& c : m_Board)
        ++counters[c.m_Suit];

    for (const auto& pair : counters)
    {
        if (pair.second >= 4)
        {
            m_Value = static_cast<Value>(m_Value | Flush);
            break;
        }
    }

    // check for flush draw
    if (!(m_Value & Flush))
    {
        for (const auto& pair : counters)
        {
            if (pair.second == 3)
            {
                m_Value = static_cast<Value>(m_Value | FlushDraw);
                break;
            }
        }
    }

    // check for pair
    Card::List hand = m_Board;
    hand.erase(std::unique(hand.begin(), hand.end(), [](const Card& lhs, const Card& rhs) { return lhs.m_Value == rhs.m_Value; }), hand.end());

    if (hand.size() != m_Board.size()) // there are duplicates
        m_Value = static_cast<Value>(m_Value | Pair);
}

std::string Board::ToString(const Value v)
{
    std::string result;

#define CASE(x) if (v & x) if (result.empty()) result += #x; else result += " | ", result += #x;
    CASE(Unknown);
    CASE(Low);
    CASE(High);
    CASE(Ace);
    CASE(StraightDraw);
    CASE(Straight);
    CASE(FlushDraw);
    CASE(Flush);
    CASE(Pair);
#undef CASE
    return result;
}

std::ostream& operator<<(std::ostream& s, Board::Value b)
{
    s << Board::ToString(b);
    return s;
}


}

