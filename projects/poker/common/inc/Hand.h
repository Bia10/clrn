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
        GoodDraw        = 1 << 10,  //!< this is a good draw but not the best
        TopDraw         = 1 << 11,  //!< hand will be nuts if draw succeed
        
        // some ready hands
        HighCard        = 1 << 12,
        Pair            = 1 << 13, 
        TwoPairs        = 1 << 14,
        ThreeOfKind     = 1 << 15,
        Straight        = 1 << 16,
        Flush           = 1 << 17,
        FullHouse       = 1 << 18,
        FourOfKind      = 1 << 19,
        StraightFlush   = 1 << 20,

        // hand strength(applicable to all hands lower than full house)
        Low             = 1 << 21,
        Middle          = 1 << 22,
        Top             = 1 << 23,

        // kickers
        LowKicker       = 1 << 24,  //!< card le than ten
        GoodKicker      = 1 << 25,  //!< picture lower then ace
        TopKicker       = 1 << 26,  //!< ace
    };
   

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
