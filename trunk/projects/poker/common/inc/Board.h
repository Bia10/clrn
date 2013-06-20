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

    //! Possible hand description
    typedef std::vector<Card::List> HandsList;

private:
    //! Cached hands type
    typedef boost::unordered_map<Hand::Value, HandsList> CachedHands;

public:

    Board(const Card::List& cards = Card::List());

    //! Get all possible cards combinations for concrete hand
    const HandsList& GetCardsByHand(const Hand::Value hand);

private:

    //! Generate possible hands
    void GeneratePossibleHands(HandsList& result, const Hand::Value hand) const;

private:

    //! Cards on the board
    Card::List m_Board;

    //! Hands cache
    static CachedHands s_HandsCache;

    //! Cache mutex
    static boost::mutex s_CacheMutex;
};

}

#endif // Board_h__
