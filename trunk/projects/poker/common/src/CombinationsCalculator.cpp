#include "CombinationsCalculator.h"
#include "Exception.h"
#include "../../evaluator/SevenEval.h"

#include <cassert>
#include <algorithm>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/random_device.hpp>

namespace clnt
{
	

Evaluator::Evaluator() : m_Evaluator(new SevenEval())
{

}

short Evaluator::GetRank(const Card::List& cards) const
{
	CHECK(cards.size() == 7, cards.size());

	int evals[7];
	for (std::size_t i = 0 ; i < cards.size(); ++i)
		evals[i] = cards[i].ToEvalFormat();

	return m_Evaluator->getRankOfSeven(evals[0], evals[1], evals[2], evals[3], evals[4], evals[5], evals[6]);
}

Evaluator::~Evaluator()
{
	delete m_Evaluator;
}

short Evaluator::GetRandomCard() const
{
	static boost::random::random_device rng;
	boost::random::uniform_int_distribution<> distribution(0, DECK_SIZE - 1);
	return distribution(rng);
}

}
