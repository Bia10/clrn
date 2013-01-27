#ifndef CombinationsCalculator_h__
#define CombinationsCalculator_h__

#include "Cards.h"
#include "Combinations.h"

#include <string>

class SevenEval;
class HandEval;

namespace pcmn
{

class Evaluator
{
public:
	enum { CARD_DECK_SIZE = 52 };
	enum { MAX_PLAYERS = 9 };

	Evaluator();
	~Evaluator();
	short GetRank(const Card::List& cards) const;
	short GetRandomCard(const short minValue = CARD_DECK_SIZE - 1) const;
	short GetRandomCard(const Card::List& dead) const;
	short GetRandomCard(const bool* dead, const short minValue = CARD_DECK_SIZE - 1) const; 
	float GetEquity(short player1, short player2, const std::vector<int>& flop, const std::vector<short>& playerRanges) const;
private:
	SevenEval* m_RanksEvaluator;
	HandEval* m_HandsEvaluator;
};
}
#endif // CombinationsCalculator_h__
