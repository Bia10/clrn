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
	enum 
	{
		NUMBER_OF_REPITITIONS = 
		1000 
#ifndef _DEBUG
		* 
		1000 
#endif
	};

	Evaluator();
	~Evaluator();
	short GetRank(const Card::List& cards) const;
	short GetRandomCard(bool* dead, const short minValue = CARD_DECK_SIZE) const; 
	float GetEquity(short player1, short player2, const std::vector<int>& flop, const std::vector<short>& playerRanges);
private:
	SevenEval* m_RanksEvaluator;
	HandEval* m_HandsEvaluator;
};
}
#endif // CombinationsCalculator_h__
