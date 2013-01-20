#ifndef CombinationsCalculator_h__
#define CombinationsCalculator_h__

#include "Cards.h"
#include "Combinations.h"

#include <string>

class SevenEval;

namespace clnt
{

class Calculator
{
public:
	Calculator();
	~Calculator();
	short GetRank(const Card::List& cards);
private:
	SevenEval* m_Evaluator;
};
}
#endif // CombinationsCalculator_h__
