#ifndef Hand_h__
#define Hand_h__

#include "Cards.h"

namespace pcmn
{

//! Player hand description
class Hand
{
public:

    //! Hand type
    enum Value
    {
        // player hand description
        Unknown         = 0,        //!< really unknown
        Suited          = 1 << 1,   //!< any suited cards
        Connectors      = 1 << 2,   //!< connectors or gub connectors
        OneHigh         = 1 << 3,   //!< one high card, more then ten        
        BothHigh        = 1 << 4,   //!< two cards more than ten
        Ace             = 1 << 5,   //!< hand with any ace
        PoketPair       = 1 << 6,   //!< pocket pair, used only on preflop(becames one of the pairs on flop+)

        // draws
        StraightDraw    = 1 << 7,   //!< two-way straight draw
        GodShot         = 1 << 8,   //!< straight godshot draw
        FlushDraw       = 1 << 9,   //!< flush draw
        
        // some ready hands
        HighCard        = 1 << 10,  //!< only high card
        LowPair         = 1 << 11,  //!< some of the pair that lower than top and middle
        MiddlePair      = 1 << 12,  //!< pair after top(or pocket pair lower than top card on the board)
        TopPair         = 1 << 13,  //!< top pair on the board(or pocket pair that higher than board)
        TwoPairs        = 1 << 14,
        ThreeOfKind     = 1 << 15,
        Straight        = 1 << 16,
        Flush           = 1 << 17,
        FullHouse       = 1 << 18,
        FourOfKind      = 1 << 19,
        StraightFlush   = 1 << 20,

        // kickers
        TopKicker       = 1 << 21,  //!< ace
        GoodKicker      = 1 << 22,  //!< picture lower then ace
        LowKicker       = 1 << 23,  //!< card le than ten
    };
   

    Hand();

    pcmn::Hand::Value GetValue() const { return m_Value; }
    void SetValue(pcmn::Hand::Value val) { m_Value = val; }

    void Parse(const pcmn::Card::List& player, const pcmn::Card::List& board);


private:

    //! Add property to the hand
    void Add(Value prop);

    //! Add kicker to the hand
    void AddKicker(const Card& card);

private:

    //! Hand type
    Value m_Value; 
};

}


#endif // Hand_h__
