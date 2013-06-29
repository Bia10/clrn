#ifndef Board_h__
#define Board_h__

#include "Cards.h"
#include "Hand.h"

#include <boost/unordered_map.hpp>
#include <boost/thread/mutex.hpp>

namespace pcmn
{

//! Table board with cards implementation
class Board
{
public:

    //! Board description
    enum Value
    {
        Unknown         = 0,
        Low             = 1 << 0,   //!< board contains only low cards
        High            = 1 << 1,   //!< board contains more than half high cards
        Ace             = 1 << 2,   //!< board contains ace
        StraightDraw    = 1 << 3,   //!< straight without two cards
        Straight        = 1 << 4,   //!< straight without one card or full straight
        FlushDraw       = 1 << 5,   //!< flush without two cards
        Flush           = 1 << 6,   //!< flush without one card or full Flush
        Pair            = 1 << 7    //!< board contains pair
    };

    //! Possible hand description
    typedef std::vector<Card::List> HandsList;

private:
    //! Cached hands type
    typedef boost::unordered_map<Hand::Value, HandsList> CachedHands;

public:

    Board(const Card::List& cards = Card::List());

    //! Get all possible cards combinations for concrete hand
    HandsList GetCardsByHand(const Hand::Value hand);

    //! Get all possible hand cards
    const HandsList& GetAllPocketCards();

    //! Parse board
    void Parse();

    //! Get board value
    Value GetValue() const { return m_Value; }

    //! To string
    static std::string ToString(const Value v);

private:

    //! Generate possible hands
    void GeneratePossibleHands(HandsList& result, const Hand::Value hand) const;

    //! Filter possible hands by board cards and hand description
    HandsList FilterCards(const HandsList& src, const Hand::Value hand);

private:

    //! Cards on the board
    Card::List m_Board;

    //! Hands cache
    static CachedHands s_HandsCache;

    //! Cache mutex
    static boost::mutex s_CacheMutex;

    //! Board value
    Value m_Value;
};

//! Stream operator
std::ostream& operator << (std::ostream& s, pcmn::Board::Value b);

}

#endif // Board_h__
