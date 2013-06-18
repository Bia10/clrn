#include "Hand.h"
#include "Exception.h"

#include <math.h>
#include <algorithm>
#include <map>

#include <boost/assign/list_of.hpp>

namespace pcmn
{
    
Hand::Value DiffFromTopToStrength(std::size_t diff, bool suited)
{
    if (suited)
    {
        if (!diff)
            return Hand::Top;
        else
        if (diff < 5)
            return Hand::Middle;
        else
            return Hand::Low;
    }
    else
    {
        switch (diff)
        {
        case 0: return Hand::Top;
        case 1: return Hand::Middle;
        default: return Hand::Low;
        }
    }
}

Hand::Value KickerFromCard(Card::Value hand, Card::Value kicker)
{
    if (kicker == Card::Ace || hand == Card::Ace && kicker == Card::King)
        return Hand::TopKicker;
    else
    if (kicker >= Card::Jack)
        return Hand::GoodKicker;
    else
        return Hand::LowKicker;
}

bool IsConnectors(const Card::List& cards)
{
    const pcmn::Card first = cards.front();
    const pcmn::Card second = cards.back();
    return first.m_Value != Card::Ace && second.m_Value != Card::Ace && abs(second.m_Value - first.m_Value) <= 2;
}

bool IsSuited(const Card::List& cards)
{
    return cards.front().m_Suit == cards.back().m_Suit;
}

bool IsOneHigh(const Card::List& cards)
{
    const pcmn::Card first = cards.front();
    const pcmn::Card second = cards.back();
    return first.m_Value >= Card::Jack && second.m_Value < Card::Jack || second.m_Value >= Card::Jack && first.m_Value < Card::Jack;
}

bool IsBothHigh(const Card::List& cards)
{
    const pcmn::Card first = cards.front();
    const pcmn::Card second = cards.back();
    return first.m_Value >= Card::Jack && second.m_Value >= Card::Jack;
}

bool IsAce(const Card::List& cards)
{
    const pcmn::Card first = cards.front();
    const pcmn::Card second = cards.back();
    return first.m_Value == Card::Ace || second.m_Value == Card::Ace;
}

bool IsPocketPair(const Card::List& cards)
{
    const pcmn::Card first = cards.front();
    const pcmn::Card second = cards.back();
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

bool IsStraight(const Card::List& cards, Hand::Value& strength, const Card::List& player, bool& draw)
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
            strength = draw ? Hand::TopDraw : Hand::Top;
        else
        if (lowCardInSequence == itFirst || lowCardInSequence == itSecond)
            strength = draw ? Hand::Unknown : Hand::Low;
        else
        if (!draw)
            strength = Hand::Middle;
            
        return true;      
    }

    return false;
}

bool IsGodShot(const Card::List& cards, const Card::List& player)
{
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

void CountCards(const Card::List& cards, std::map<Card::Value, unsigned>& counters)
{
    for (const Card& c : cards)
        ++counters[c.m_Value];
}

bool IsFlush(const Card::List& cards, const Card::List& player, Hand::Value& strength)
{
    std::map<Suit::Value, unsigned> counters;
    std::map<Suit::Value, Card> highCards;
    CountSuites(cards, counters, highCards);

    const pcmn::Card first = player.front();
    const pcmn::Card second = player.back();

    for (const auto& pair : counters)
    {
        if (pair.second < 5)
            continue;

        // flush suit
        const Suit::Value suit = pair.first;

        // get all cards of this suit
        Card::List suited;
        suited.reserve(7);
        std::copy_if(cards.begin(), cards.end(), std::back_inserter(suited), [&](const Card& c) { return c.m_Suit == suit; });

        // sort by card rank
        std::sort(suited.begin(), suited.end(), [](const Card& lhs, const Card& rhs) { return lhs.m_Value > rhs.m_Value; });

        // remove low elements
        suited.resize(5);

        // find out player cards
        const Card::List::const_iterator firstCard = std::find(suited.begin(), suited.end(), first);
        const Card::List::const_iterator secondCard = std::find(suited.begin(), suited.end(), second);

        if (firstCard == suited.end() && secondCard == suited.end())
            continue; // flush on the board

        Card::Value best = Card::Unknown;
        if (firstCard != suited.end() && secondCard != suited.end())
            best = std::max(firstCard->m_Value, secondCard->m_Value);
        else
        if (firstCard != suited.end())
            best = firstCard->m_Value;
        else
            best = secondCard->m_Value;

        // flush strength
        const std::size_t diff = Card::Ace - best;
        strength = DiffFromTopToStrength(diff, true);

        return true;
    }
    return false;
}

bool IsFlushDraw(const Card::List& cards, const Card::List& player, Hand::Value& strength)
{
    std::map<Suit::Value, unsigned> counters;
    std::map<Suit::Value, Card> highCards;
    CountSuites(cards, counters, highCards);

    const pcmn::Card first = player.front();
    const pcmn::Card second = player.back();

    for (const auto& pair : counters)
    {
        if (pair.second != 4)
            continue;

        // flush suit
        const Suit::Value suit = pair.first;

        // get all cards of this suit
        Card::List suited;
        suited.reserve(7);
        std::copy_if(cards.begin(), cards.end(), std::back_inserter(suited), [&](const Card& c) { return c.m_Suit == suit; });

        // sort by card rank
        std::sort(suited.begin(), suited.end(), [](const Card& lhs, const Card& rhs) { return lhs.m_Value > rhs.m_Value; });

        // remove low elements
        suited.resize(4);

        // find out player cards
        const Card::List::const_iterator firstCard = std::find(suited.begin(), suited.end(), first);
        const Card::List::const_iterator secondCard = std::find(suited.begin(), suited.end(), second);

        if (firstCard == suited.end() && secondCard == suited.end())
            continue; // flush on the board

        // straight strength
        Card::Value best = Card::Unknown;
        if (firstCard != suited.end() && secondCard != suited.end())
            best = std::max(firstCard->m_Value, secondCard->m_Value);
        else
        if (firstCard != suited.end())
            best = firstCard->m_Value;
        else
            best = secondCard->m_Value;

        // flush strength
        const std::size_t diff = Card::Ace - best;
        strength = DiffFromTopToStrength(diff, true);
        switch (strength)
        {
        case Hand::Top: strength = Hand::TopDraw; break;
        case Hand::Middle: strength = Hand::GoodDraw; break;
        default: strength = Hand::Unknown; break;
        }

        return true;
    }
    return false;
}

bool IsStraightFlush(const Card::List& cards, const Card::List& player)
{
    std::vector<Card::List> sequences;
    ExtractOrdered(cards, sequences);

    const pcmn::Card first = player.front();
    const pcmn::Card second = player.back();

    for (unsigned i = 0 ; i < sequences.size(); ++i)
    {
        pcmn::Card::List& sequence = sequences[i];
        if (sequence.size() < 5)
            continue;

        const Card::List::const_iterator itFirst = std::find(sequence.begin(), sequence.end(), player.front());
        const Card::List::const_iterator itSecond = std::find(sequence.begin(), sequence.end(), player.back());
        if (itFirst == sequence.end() && itSecond == sequence.end())
            continue; // player hand doesn't play

        // copy player cards to sequence
        if (std::find_if(sequence.begin(), sequence.end(), [&](const Card& c) { return c.m_Value == first.m_Value; }) != sequence.end())
            sequence.push_back(first);

        if (std::find_if(sequence.begin(), sequence.end(), [&](const Card& c) { return c.m_Value == second.m_Value; }) != sequence.end())
            sequence.push_back(second);

        std::sort(sequence.begin(), sequence.end(), [](const Card& lhs, const Card& rhs) { return lhs.m_Value < rhs.m_Value; });

        // erase duplicates
        sequence.erase(std::unique(sequence.begin(), sequence.end(), [](const Card& lhs, const Card& rhs) { return lhs == rhs; }), sequence.end());

        std::map<Suit::Value, unsigned> counters;
        std::map<Suit::Value, Card> highCards;
        CountSuites(sequence, counters, highCards);

        for (const auto& pair : counters)
        {
            if (pair.second >= 5)
                return true;
        }
    }

    return false;
}

bool IsXOfKind(const Card::List& cards, const Card::List& player, unsigned count, Hand::Value& value, Hand::Value& kicker)
{
    std::map<Card::Value, unsigned> counters;
    CountCards(cards, counters);

    const pcmn::Card first = player.front();
    const pcmn::Card second = player.back();

    for (const auto& pair : counters)
    {
        if (pair.second != count || (player.front().m_Value != pair.first && player.back().m_Value != pair.first))
            continue;

        Card::List board;
        std::copy_if(cards.begin(), cards.end(), std::back_inserter(board), [&](const Card& c){ return !(c == first || c == second); });

        // detect strength
        const Card::List::const_reverse_iterator it = std::find_if(board.rbegin(), board.rend(), [&](const Card& c) { return c.m_Value == pair.first; });
        assert(it != board.rend());

        // difference from top card
        const std::size_t diff = std::distance<Card::List::const_reverse_iterator>(board.rbegin(), it);
        value = DiffFromTopToStrength(diff, false);

        // kicker
        Card::Value otherCard = Card::Unknown;
        if (first.m_Value != pair.first)
            otherCard = first.m_Value;
        else
        if (second.m_Value != pair.first)
            otherCard = second.m_Value;
        else
            return true; // this is pocket pair, no kicker

        kicker = KickerFromCard(pair.first, otherCard);
        return true;
    }

    return false;
}

bool IsFullHouse(const Card::List& cards, const Card::List& player)
{
    std::map<Card::Value, unsigned> counters;
    CountCards(cards, counters);

    const pcmn::Card first = player.front();
    const pcmn::Card second = player.back();

    Card::Value pair = Card::Unknown;
    Card::Value trips = Card::Unknown;

    for (const auto& hand : counters)
    {
        if (pair == Card::Unknown && hand.second == 2)
        {
            pair = hand.first;
            if (trips != Card::Unknown)
                break;
        }

        if (trips == Card::Unknown && hand.second == 3)
        {
            trips = hand.first;
            if (pair != Card::Unknown)
                break;
        }
    }

    return pair != Card::Unknown && trips != Card::Unknown && 
        (first.m_Value == pair || first.m_Value == trips || second.m_Value == pair || second.m_Value == trips);
}

bool IsTwoPairs(const Card::List& cards, const Card::List& player, Hand::Value& value, Hand::Value& kicker)
{
    std::map<Card::Value, unsigned> counters;
    CountCards(cards, counters);

    const pcmn::Card first = player.front();
    const pcmn::Card second = player.back();

    Card::Value pair1 = Card::Unknown;
    Card::Value pair2 = Card::Unknown;

    const auto end = counters.rend();
    auto it = counters.rbegin();

    for (; it != end; ++it)
    {
        const auto& hand = *it;
        if (pair1 == Card::Unknown && hand.second == 2)
        {
            pair1 = hand.first;
        }
        else
        if (pair2 == Card::Unknown && hand.second == 2)
        {
            pair2 = hand.first;
            break;
        }
    }

    const bool isTwoPairs = pair1 != Card::Unknown && pair2 != Card::Unknown && 
        (first.m_Value == pair1 || first.m_Value == pair2 || second.m_Value == pair1 || second.m_Value == pair2);

    if (isTwoPairs)
    {
        // detect strength of the best pair
        std::vector<Card::Value> playerCards;
        if (pair1 == first.m_Value || pair1 == second.m_Value)
            playerCards.push_back(pair1);

        if (pair2 == first.m_Value || pair2 == second.m_Value)
            playerCards.push_back(pair2);

        assert(!playerCards.empty());
        std::sort(playerCards.begin(), playerCards.end());

        const Card::Value bestpair = playerCards.back();

        Card::List unique;
        std::unique_copy(cards.begin(), cards.end(), std::back_inserter(unique), [](const Card& lhs, const Card& rhs) { return lhs.m_Value == rhs.m_Value; });

        const Card::List::const_reverse_iterator it = std::find_if(unique.rbegin(), unique.rend(), [&](const Card& c) { return c.m_Value == bestpair; });
        assert(it != unique.rend());

        // difference from top card
        const std::size_t diff = std::distance<Card::List::const_reverse_iterator>(unique.rbegin(), it);
        value = DiffFromTopToStrength(diff, false);

        // kicker
        Card kickerCard;
        if (first.m_Value != second.m_Value)
        {
            if ((first.m_Value == pair1 || first.m_Value == pair2) && second.m_Value != pair1 && second.m_Value != pair2)
                kickerCard = second;
            else
            if ((second.m_Value == pair1 || second.m_Value == pair2) && first.m_Value != pair1 && first.m_Value != pair2)
                kickerCard = first;
        }

        kicker = KickerFromCard(pair1, kickerCard.m_Value);
    }

    return isTwoPairs;
}

bool IsPair(const Card::List& cards, const Card::List& player, Hand::Value& result, Hand::Value& kicker)
{
    std::map<Card::Value, unsigned> counters;
    CountCards(cards, counters);

    const pcmn::Card first = player.front();
    const pcmn::Card second = player.back();

    for (const auto& hand : counters)
    {
        if (hand.second == 2 && (hand.first == first.m_Value || hand.first == second.m_Value))
        {
            Card::List board;
            if (hand.first != first.m_Value || hand.first != second.m_Value)
            {
                // this is not a pocket pair, it should be compared with board only
                std::copy_if(cards.begin(), cards.end(), std::back_inserter(board), [&](const Card& c) { return !(c == first || c == second); });
            }
            else
            {
                std::unique_copy(cards.begin(), cards.end(), std::back_inserter(board), [](const Card& lhs, const Card& rhs) { return lhs.m_Value == rhs.m_Value; });
            }

            // found pair, calculate pair value
            const Card::List::const_reverse_iterator it = std::find_if(board.rbegin(), board.rend(), [&](const Card& c) { return c.m_Value == hand.first; });
            assert(it != board.rend());

            // difference from top card
            const std::size_t diff = std::distance<Card::List::const_reverse_iterator>(board.rbegin(), it);
            result = DiffFromTopToStrength(diff, false);

            // find out kicker
            Card::Value kickerCard = Card::Unknown;
            if (first.m_Value != hand.first)
                kickerCard = first.m_Value;
            else
            if (second.m_Value != hand.first)
                kickerCard = second.m_Value;

            kicker = KickerFromCard(hand.first, kickerCard);
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
    CHECK(player.size() == 2, "Invalid hand! Must be two cards", player.size());
    
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

    // sort by value
    std::sort(hand.begin(), hand.end(), [](const Card& lhs, const Card& rhs) { return lhs.m_Value < rhs.m_Value; });

    hand.erase(std::unique(hand.begin(), hand.end(), [](const Card& lhs, const Card& rhs) { return lhs == rhs; }), hand.end());
    CHECK(hand.size() == boardInput.size() + player.size(), "Cards are duplicated", player, boardInput);

    // check hand
    Value kicker = Unknown;
    Value strength = Unknown;
    Value straightStrength = Unknown;
    if (IsStraightFlush(hand, player))
    {
        Add(StraightFlush);
        return;
    }
    if (IsXOfKind(hand, player, 4, strength, kicker))
    {
        Add(FourOfKind);
        return;
    }
    if (IsFullHouse(hand, player))
    {
        Add(FullHouse);
        return;
    }

    bool straightDraw = false;
    const bool isFlush = IsFlush(hand, player, strength);
    const bool isStraight = IsStraight(hand, straightStrength, player, straightDraw);

    if (isFlush)
    {
        Add(Flush);
        Add(strength);
        return;
    }

    if (isStraight && !straightDraw)
    {
        Add(Straight);
        Add(straightStrength);
        return;
    }
    if (IsGodShot(hand, player))
    {
        Add(GodShot);
    }

    if (IsXOfKind(hand, player, 3, strength, kicker))
    {
        Add(ThreeOfKind);
        Add(strength);
        Add(kicker);
    }
    else
    if (IsTwoPairs(hand, player, strength, kicker))
    {
        Add(TwoPairs);
        Add(strength);
        Add(kicker);
    }
    else
    if (IsPair(hand, player, strength, kicker))
    {
        Add(Pair);
        Add(strength);
        Add(kicker);
    }
    else
    {
        Add(HighCard);

        if (!(m_Value & LowKicker || m_Value & GoodKicker || m_Value & TopKicker))
            AddKicker(std::max(player.front().m_Value, player.back().m_Value));
    }

    if (!isFlush && IsFlushDraw(hand, player, strength))
    {
        Add(FlushDraw);
        Add(strength);
    }

    if (!isFlush && isStraight && straightDraw)
    {
        Add(StraightDraw);
        Add(straightStrength);
    }
}

void Hand::Add(Value prop)
{
    m_Value = static_cast<pcmn::Hand::Value>(int(m_Value) | prop);
}

void Hand::AddKicker(const Card::Value card)
{
    if (card == Card::Ace)
        Add(TopKicker);
    else
    if (card >= Card::Jack)
        Add(GoodKicker);
    else
        Add(LowKicker);
}

std::string Hand::ToString(Value v)
{
    std::string result;

#define CASE(x) if (v & x) if (result.empty()) result += #x; else result += " | ", result += #x;
    CASE(Unknown);
    CASE(Suited);
    CASE(Connectors);
    CASE(OneHigh);
    CASE(BothHigh);
    CASE(Ace);
    CASE(PoketPair);
    CASE(StraightDraw);
    CASE(GodShot);
    CASE(FlushDraw);
    CASE(HighCard);
    CASE(TwoPairs);
    CASE(ThreeOfKind);
    CASE(Straight);
    CASE(Flush);
    CASE(FullHouse);
    CASE(FourOfKind);
    CASE(StraightFlush);
    CASE(Low);
    CASE(Middle);
    CASE(Top);
    CASE(TopKicker);
    CASE(GoodKicker);
    CASE(LowKicker);
#undef CASE
    return result;
}

std::ostream& operator<<(std::ostream& s, const Hand::Value h)
{
    s << Hand::ToString(h);
    return s;
}

}