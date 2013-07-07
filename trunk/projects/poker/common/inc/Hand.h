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
        OneLow          = 1 << 1,   //!< one card is low (lower than 6)
        BothLow         = 1 << 2,   //!, both cards low
        Suited          = 1 << 3,   //!< any suited cards
        Connectors      = 1 << 4,   //!< connectors or gub connectors
        OneHigh         = 1 << 5,   //!< one high card, more then ten        
        BothHigh        = 1 << 6,   //!< two cards more than ten
        Ace             = 1 << 7,   //!< hand with any ace
        PoketPair       = 1 << 8,   //!< pocket pair, used only on preflop(becames one of the pairs on flop+)

        // draws
        StraightDraw    = 1 << 9,   //!< two-way straight draw
        GutShot         = 1 << 10,   //!< straight gutshot draw
        FlushDraw       = 1 << 11,   //!< flush draw
        GoodDraw        = 1 << 12,  //!< this is a good draw but not the best
        TopDraw         = 1 << 13,  //!< hand will be nuts if draw succeed
        
        // some ready hands
        HighCard        = 1 << 14,  //!< means overcard only
        Pair            = 1 << 15,
        TwoPairs        = 1 << 16,
        ThreeOfKind     = 1 << 17,
        Straight        = 1 << 18,
        Flush           = 1 << 19,
        FullHouse       = 1 << 20,
        FourOfKind      = 1 << 21,
        StraightFlush   = 1 << 22,

        // hand strength(applicable to all hands lower than full house)
        Low             = 1 << 23,
        Middle          = 1 << 24,
        Top             = 1 << 25,

        // kickers
        LowKicker       = 1 << 26,  //!< card le than ten
        GoodKicker      = 1 << 27,  //!< picture lower then ace
        TopKicker       = 1 << 28,  //!< ace
    };

    static const unsigned POCKET_HAND_MASK;
    static const unsigned FLOP_HAND_MASK;
    static const unsigned KICKERS_MASK;
    static const unsigned DRAWS_MASK;
    static const unsigned POWER_MASK;
   
    Hand();

    pcmn::Hand::Value GetValue() const { return m_Value; }
    void SetValue(pcmn::Hand::Value val) { m_Value = val; }

    void Parse(const pcmn::Card::List& player, const pcmn::Card::List& board);

    static std::string ToString(Value v);


private:

    //! Add property to the hand
    void Add(Value prop);

    //! Add kicker to the hand
    void AddKicker(const Card::Value card);

private:

    //! Hand type
    Value m_Value; 
};

//! Stream operator
std::ostream& operator << (std::ostream& s, pcmn::Hand::Value h);

}


#endif // Hand_h__
