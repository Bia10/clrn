#include "Hand.h"
#include "Exception.h"

#include <math.h>
#include <algorithm>
#include <map>

#include <boost/assign/list_of.hpp>

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

bool IsOneHigh(const Card::List& cards)
{
    const pcmn::Card first = *cards.begin();
    const pcmn::Card second = *(cards.begin() + 1);
    return first.m_Value >= Card::Jack && second.m_Value < Card::Jack || second.m_Value >= Card::Jack && first.m_Value < Card::Jack;
}

bool IsBothHigh(const Card::List& cards)
{
    const pcmn::Card first = *cards.begin();
    const pcmn::Card second = *(cards.begin() + 1);
    return first.m_Value >= Card::Jack && second.m_Value >= Card::Jack;
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

void ExtractOrdered(const Card::List& input, std::vector<pcmn::Card::List>& result)
{
    Card::List cards;
    std::unique_copy(input.begin(), input.end(), std::back_inserter(cards), [](const Card& lhs, const Card& rhs) { return lhs.m_Value == rhs.m_Value; });

    result.push_back(Card::List());

    const bool aceExists = cards.back().m_Value == Card::Ace;
    unsigned cardsBetween = 0;

    Card current;
    Card next;
    for (unsigned i = 0 ; i < (aceExists ? cards.size() : cards.size() - 1); ++i)
    {
        const bool compareWithAce = (!i && aceExists);

        current = aceExists ? (i ? cards[i - 1] : cards.back()) : cards[i];
        next = aceExists ? (i ? cards[i] : cards.front()) : cards[i + 1];

        if (compareWithAce && next.m_Value == Card::Two || abs(current.m_Value - next.m_Value) == 1)
        {
            if (result.back().empty())
                result.back().push_back(current);
            result.back().push_back(next);
            cardsBetween = 0;
        }
        else
        {
            if (!cardsBetween)
                result.push_back(boost::assign::list_of(next));
            else
                result.back().clear();

            ++cardsBetween;
        }
    }
}

bool IsStraight(const Card::List& cards, Hand::Value& highCard, const Card::List& player, bool& draw)
{
    std::vector<Card::List> sequences;
    ExtractOrdered(cards, sequences);

    draw = false;
    for (unsigned i = 0 ; i < sequences.size(); ++i)
    {
        const pcmn::Card::List& sequence = sequences[i];
        if (sequence.size() < 4)
            continue;

        if (sequence.size() == 4)
        {
            if (sequence.front().m_Value != Card::Ace && sequence.back().m_Value != Card::Ace)
                draw = true;
            else
                continue;
        }

        const Card::List::const_iterator itFirst = std::find(sequence.begin(), sequence.end(), player.front());
        const Card::List::const_iterator itSecond = std::find(sequence.begin(), sequence.end(), player.back());
        if (itFirst == sequence.end() && itSecond == sequence.end())
            continue; // draw on the board
            
        const Card::List::const_iterator highCardInSequence = sequence.end() - 1;
        const Card::List::const_iterator lowCardInSequence = sequence.begin();

        if (highCardInSequence == itFirst || highCardInSequence == itSecond)
            highCard = Hand::TopKicker;
        else
        if (lowCardInSequence == itFirst || lowCardInSequence == itSecond)
            highCard = Hand::LowKicker;
        return true;      
    }

    return false;
}

bool IsGodShot(const Card::List& cards, const Card::List& player)
{
    // TODO: add high card calculation
    std::vector<Card::List> sequences;
    ExtractOrdered(cards, sequences);

    for (unsigned i = 0 ; i < sequences.size() - 1; ++i)
    {
        const Card::List& current = sequences[i];
        const Card::List& next = sequences[i + 1];

        if 
        (
            std::find(current.begin(), current.end(), player.front()) == current.end() 
            && 
            std::find(current.begin(), current.end(), player.back()) == current.end()
            &&
            std::find(next.begin(), next.end(), player.front()) == next.end() 
            && 
            std::find(next.begin(), next.end(), player.back()) == next.end()
         )
            continue; // god shot on board

        if (current.size() + next.size() >= 4 && !current.empty() && !next.empty() && next.front().m_Value - current.back().m_Value == 2)
            return true; // god shot in the middle

        if (current.size() == 4 && (current.back().m_Value == Card::Ace || current.front().m_Value == Card::Ace))
            return true; // god shot from the beginning or the end

        if (next.size() == 4 && (next.back().m_Value == Card::Ace || next.front().m_Value == Card::Ace))
            return true; // god shot from the beginning or the end
    }

    return false;
}

void CountSuites(const Card::List& cards, std::map<Suit::Value, unsigned>& counters, std::map<Suit::Value, Card>& highCards)
{
    for (const Card& c : cards)
    {
        ++counters[c.m_Suit];
        if (highCards[c.m_Suit].m_Value < c.m_Value)
            highCards[c.m_Suit] = c;
    }
}

bool IsFlush(const Card::List& cards, Card& high, const Card::List& player)
{
    std::map<Suit::Value, unsigned> counters;
    std::map<Suit::Value, Card> highCards;
    CountSuites(cards, counters, highCards);

    for (const auto& pair : counters)
    {
        if (pair.second >= 5)
        {
            high = highCards[pair.first];
            return true;
        }
    }
    return false;
}

bool IsFlushDraw(const Card::List& cards, Card& high, const Card::List& player)
{
    std::map<Suit::Value, unsigned> counters;
    std::map<Suit::Value, Card> highCards;
    CountSuites(cards, counters, highCards);
    for (const auto& pair : counters)
    {
        if (pair.second == 4)
        {
            high = highCards[pair.first];
            return true;
        }
    }
    return false;
}

Hand::Hand() : m_Value(Unknown)
{

}

void Hand::Parse(const pcmn::Card::List& player, const pcmn::Card::List& boardInput)
{
    CHECK(player.size() == 2, "Invalid hand must be two cards", player.size());
    
    m_Value = Unknown;

    // parse player hand first
    if (IsSuited(player))
        Add(Suited);

    if (IsConnectors(player))
        Add(Connectors);
    
    if (IsOneHigh(player))
        Add(OneHigh);

    if (IsBothHigh(player))
        Add(BothHigh);

    if (IsAce(player))
        Add(Ace);

    if (IsPocketPair(player))
        Add(PoketPair);

    if (boardInput.empty())
        return;

    CHECK(boardInput.size() >= 3 && boardInput.size() <= 5, "Invalid board must be 3, 4 or 5 cards", boardInput.size());

    // now parse hands with board
    pcmn::Card::List hand(player.begin(), player.end());
    std::copy(boardInput.begin(), boardInput.end(), std::back_inserter(hand));

    //pcmn::Card::List board(boardInput);
    //std::sort(board.begin(), board.end(), [](const Card& lhs, const Card& rhs) { return lhs.m_Value < rhs.m_Value; });

    // sort by value
    std::sort(hand.begin(), hand.end(), [](const Card& lhs, const Card& rhs) { return lhs.m_Value < rhs.m_Value; });

    // check hand
    Card highFlush;
    Value highStraight = Unknown;
    bool straightDraw = false;
    const bool isFlush = IsFlush(hand, highFlush, player);
    const bool isStraight = IsStraight(hand, highStraight, player, straightDraw);

    if (isFlush && isStraight && !straightDraw)
    {
        Add(StraightFlush);
        Add(highStraight);
    }
    else
    if (isFlush)
    {
        Add(Flush);
        AddKicker(highFlush);
    }
    else
    if (isStraight && !straightDraw)
    {
        Add(Straight);
        Add(highStraight);
    }
    else
    if (IsGodShot(hand, player))
    {
        Add(GodShot);
    }

    if (!isFlush && IsFlushDraw(hand, highFlush, player))
    {
        Add(FlushDraw);
        AddKicker(highFlush);
    }

    if (!isFlush && isStraight && straightDraw)
    {
        Add(StraightDraw);
        Add(highStraight);
    }
}

void Hand::Add(Value prop)
{
    m_Value = static_cast<pcmn::Hand::Value>(int(m_Value) | prop);
}

void Hand::AddKicker(const Card& card)
{
    if (card.m_Value == Card::Ace)
        Add(TopKicker);
    else
    if (card.m_Value >= Card::Jack)
        Add(GoodKicker);
    else
        Add(LowKicker);
}

}