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
        Trash           = 1 << 1,   //!< something small like 3-6 or 2-7
        Suited          = 1 << 2,   //!< any suited cards
        Connectors      = 1 << 3,   //!< connectors or gub connectors
        Ace             = 1 << 4,   //!< hand with any ace
        PoketPair       = 1 << 5,   //!< pocket pair, used only on preflop(becames one of the pairs on flop+)

        // draws
        StraightDraw    = 1 << 6,   //!< two-way straight draw
        GodShot         = 1 << 7,   //!< straight godshot draw
        FlushDraw       = 1 << 8,   //!< flush draw
        
        // some ready hands
        HighCard        = 1 << 9,   //!< only high card
        LowPair         = 1 << 10,  //!< some of the pair that lower than top and middle
        MiddlePair      = 1 << 11,  //!< pair after top(or pocket pair lower than top card on the board)
        TopPair         = 1 << 12,  //!< top pair on the board(or pocket pair that higher than board)
        TwoPairs        = 1 << 13,
        ThreeOfKind     = 1 << 14,
        Straight        = 1 << 15,
        Flush           = 1 << 16,
        FullHouse       = 1 << 17,
        FourOfKind      = 1 << 18,
        StraightFlush   = 1 << 19,

        // kickers
        TopKicker       = 1 << 20,  //!< ace
        GoodKicker      = 1 << 21,  //!< picture lower then ace
        LowKicker       = 1 << 22,  //!< card le than ten
    };
   

    Hand();

    pcmn::Hand::Value GetValue() const { return m_Value; }
    void SetValue(pcmn::Hand::Value val) { m_Value = val; }

    void Parse(const pcmn::Card::List& player, const pcmn::Card::List& board);


private:

    //! Add property to the hand
    void Add(Value prop);

private:

    //! Hand type
    Value m_Value; 
};

}


#endif // Hand_h__
