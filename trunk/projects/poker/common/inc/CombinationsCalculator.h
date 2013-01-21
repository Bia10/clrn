#ifndef CombinationsCalculator_h__
#define CombinationsCalculator_h__

#include "Cards.h"
#include "Combinations.h"

#include <string>

class SevenEval;

namespace clnt
{

class Evaluator
{
public:
	Evaluator();
	~Evaluator();
	short GetRank(const Card::List& cards) const;
	short GetRandomCard() const;
private:
	SevenEval* m_Evaluator;
};
}
#endif // CombinationsCalculator_h__
