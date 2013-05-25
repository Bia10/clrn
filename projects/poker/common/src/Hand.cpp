#include "Hand.h"
#include "Exception.h"

#include <math.h>
#include <algorithm>
#include <map>

namespace pcmn
{
    
bool IsConnectors(const Card::List& cards)
{
    const pcmn::Card first = *cards.begin();
    const pcmn::Card second = *(cards.begin() + 1);
    return first.m_Value != Card::Ace && second.m_Value != Card::Ace && abs(second.m_Value - first.m_Value) <= 2;
}

bool IsSuited(const Card::List& cards)
{
    return cards.begin()->m_Suit == (cards.begin() + 1)->m_Suit;
}

bool IsTrash(const Card::List& cards)
{
    return cards.front().m_Value < Card::Jack && cards.back().m_Value < Card::Jack;
}

bool IsAce(const Card::List& cards)
{
    const pcmn::Card first = *cards.begin();
    const pcmn::Card second = *(cards.begin() + 1);
    return first.m_Value == Card::Ace || second.m_Value == Card::Ace;
}

bool IsPocketPair(const Card::List& cards)
{
    const pcmn::Card first = *cards.begin();
    const pcmn::Card second = *(cards.begin() + 1);
    return first.m_Value == second.m_Value;
}

void CountOrdered(const Card::List& cards, std::vector<unsigned>& counters)
{
    const bool aceExists = cards.back().m_Value == Card::Ace;
    counters.push_back(0);
    unsigned cardsBetween = 0;

    for (unsigned i = 0 ; i < cards.size() - 1; ++i)
    {
        const bool compareWithAce = (!i && aceExists);

        const Card current = compareWithAce ? cards.back() : cards[i];
        const Card next = compareWithAce ? cards.front() : cards[i + 1];

        if (compareWithAce && next.m_Value == Card::Two || abs(current.m_Value - next.m_Value) == 1)
        {
            ++counters.back();
            cardsBetween = 0;
        }
        else
        {
            if (!cardsBetween)
                counters.push_back(0);
            else
                counters.back() = 0;

            ++cardsBetween;
        }
    }
}

bool IsStraightDraw(const Card::List& cards)
{
    std::vector<unsigned> counters;
    CountOrdered(cards, counters);

    for (const unsigned c : counters)
    {
        if (c == 4)
            return true;
    }

    return false;
}

bool IsStraight(const Card::List& cards)
{
    std::vector<unsigned> counters;
    CountOrdered(cards, counters);

    for (const unsigned c : counters)
    {
        if (c >= 5)
            return true;
    }

    return false;
}

bool IsGodShot(const Card::List& cards)
{
    std::vector<unsigned> counters;
    CountOrdered(cards, counters);

    std::sort(counters.begin(), counters.end());
    std::unique(counters.begin(), counters.end());

    return counters.size() > 1 && counters.front() + counters.back() == 4;
}

void CountSuites(const Card::List& cards, std::map<Suit::Value, unsigned>& counters)
{
    for (const Card& c : cards)
        ++counters[c.m_Suit];
}

bool IsFlush(const Card::List& cards)
{
    std::map<Suit::Value, unsigned> counters;
    CountSuites(cards, counters);
    return counters[Suit::Hearts] >= 5 || counters[Suit::Clubs] >= 5 || counters[Suit::Spades] >= 5 || counters[Suit::Diamonds] >= 5;
}

bool IsFlushDraw(const Card::List& cards)
{
    std::map<Suit::Value, unsigned> counters;
    CountSuites(cards, counters);
    return counters[Suit::Hearts] == 4 || counters[Suit::Clubs] == 4 || counters[Suit::Spades] == 4 || counters[Suit::Diamonds] == 4;
}

Hand::Hand() : m_Value(Unknown)
{

}

void Hand::Parse(const pcmn::Card::List& player, const pcmn::Card::List& board)
{
    CHECK(player.size() == 2, "Invalid hand must be two cards", player.size());
    
    m_Value = Unknown;

    // parse player hand first
    const bool connectors = IsConnectors(player);
    const bool suited = IsSuited(player);

    if (!suited && !connectors && IsTrash(player))
        Add(Trash);
    else
    {
        if (suited)
            Add(Suited);
        if (connectors)
            Add(Connectors);
    }

    if (IsAce(player))
        Add(Ace);

    if (IsPocketPair(player))
        Add(PoketPair);

    if (board.empty())
        return;

    CHECK(board.size() >= 3 && board.size() <= 5, "Invalid board must be 3, 4 or 5 cards", board.size());

    // now parse hands with board
    pcmn::Card::List hand(player.begin(), player.end());
    std::copy(board.begin(), board.end(), std::back_inserter(hand));

    // sort by value
    std::sort(hand.begin(), hand.end(), [](const Card& lhs, const Card& rhs) { return lhs.m_Value < rhs.m_Value; });

    // check hand
    if (IsFlush(hand))
        Add(Flush);
    else
    if (IsFlushDraw(hand))
        Add(FlushDraw);

    if (IsStraight(hand))
        Add(Straight);
    else
    if (IsStraightDraw(hand))
        Add(StraightDraw);
    else
    if (IsGodShot(hand))
        Add(GodShot);




}

void Hand::Add(Value prop)
{
    m_Value = static_cast<pcmn::Hand::Value>(int(m_Value) | prop);
}

}