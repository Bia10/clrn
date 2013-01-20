#include "CombinationsCalculator.h"
#include "Exception.h"
#include "../../evaluator/SevenEval.h"

#include <cassert>
#include <algorithm>

namespace clnt
{
	

Calculator::Calculator() : m_Evaluator(new SevenEval())
{

}

short Calculator::GetRank(const Card::List& cards)
{
	CHECK(cards.size() == 7, cards.size());

	int evals[7];
	for (std::size_t i = 0 ; i < cards.size(); ++i)
		evals[i] = cards[i].ToEvalFormat();

	return m_Evaluator->getRankOfSeven(evals[0], evals[1], evals[2], evals[3], evals[4], evals[5], evals[6]);
}

Calculator::~Calculator()
{
	delete m_Evaluator;
}

}
