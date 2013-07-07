#ifndef Evaluator_h__
#define Evaluator_h__

#include "Cards.h"
#include "Combinations.h"
#include "Config.h"
#include "Hand.h"

#include <string>

class SevenEval;
class HandEval;

namespace pcmn
{

class Evaluator
{
public:
	Evaluator(unsigned repititions = cfg::NUMBER_OF_REPITITIONS);
	~Evaluator();
	short GetRank(const Card::List& cards) const;
	short GetRandomCard(bool* dead, const short minValue = cfg::CARD_DECK_SIZE) const; 
	float GetEquity(short player1, short player2, const std::vector<int>& flop, const std::vector<short>& playerRanges) const;
    float GetEquity(short player1, short player2, const Card::List& flop, Hand::Value opponent) const;
private:
	SevenEval* m_RanksEvaluator;
	HandEval* m_HandsEvaluator;
    unsigned m_Repititions;
};
}
#endif // Evaluator_h__
