#include "Evaluator.h"
#include "Exception.h"
#include "../../evaluator/SevenEval.h"
#include "../../evaluator/HandEval.h"

#include <cassert>
#include <algorithm>

#include <boost/date_time.hpp>

namespace pcmn
{

Evaluator::Evaluator() : m_RanksEvaluator(new SevenEval()), m_HandsEvaluator(new HandEval())
{
	srand ( ( unsigned int ) time ( 0 ) );
}

Evaluator::~Evaluator()
{
	delete m_RanksEvaluator;
	delete m_HandsEvaluator;
}

short Evaluator::GetRank(const Card::List& cards) const
{
	CHECK(cards.size() == 7, cards.size());

	int evals[7];
	for (std::size_t i = 0 ; i < cards.size(); ++i)
		evals[i] = cards[i].ToEvalFormat();

	return m_RanksEvaluator->getRankOfSeven(evals[0], evals[1], evals[2], evals[3], evals[4], evals[5], evals[6]);
}

short Evaluator::GetRandomCard(bool* dead, const short minValue) const
{
	for (;;)
	{
		const short card = rand() % minValue;
		if (!dead[card])
		{
			dead[card] = true;
			return card;
		}
	}
}

float Evaluator::GetEquity(short player1, short player2, const std::vector<int>& flop, const std::vector<short>& playerRanges) const
{
	bool deadCards[cfg::CARD_DECK_SIZE] = {false};

	deadCards[player1] = true;
	deadCards[player2] = true;

	// get flop cards
	int flopCards[5];
	std::size_t flopCardsCount = 0;
	for ( ; flopCardsCount < flop.size(); ++flopCardsCount)
	{
		const int temp = flop[flopCardsCount];
		flopCards[flopCardsCount] = temp;
		deadCards[temp] = true;
	}

	std::size_t looses = 0;
	std::vector<int> outs;
	for (std::size_t repetition = 0 ; repetition < cfg::NUMBER_OF_REPITITIONS; ++repetition)
	{
		outs.clear();

		// get random cards for flop
		for (std::size_t i = flopCardsCount; i < 5; ++i)
		{
			const int temp =  GetRandomCard(deadCards);
			flopCards[i] = temp;
			deadCards[temp] = true;
			outs.push_back(temp);
		}

		// get bot rank
		const short botRank = m_RanksEvaluator->getRankOfSeven(player1, player2, flopCards[0], flopCards[1], flopCards[2], flopCards[3], flopCards[4]);

		// get random cards for each player
		for (std::size_t i = 0; i < playerRanges.size(); ++i)
		{
			const int first = GetRandomCard(deadCards, playerRanges[i]);
			deadCards[first] = true;
			const int second = GetRandomCard(deadCards, playerRanges[i]);
			deadCards[second] = true;

			outs.push_back(first);
			outs.push_back(second);

			// get player rank
			const short playerRank = m_RanksEvaluator->getRankOfSeven(first, second, flopCards[0], flopCards[1], flopCards[2], flopCards[3], flopCards[4]);
			if (playerRank >= botRank)
			{
				++looses;
				break;
			}
		}

		// clean up dead cards
		for (const int card : outs)
			deadCards[card] = false;
	}

	return static_cast<float>(100.0f - (static_cast<double>(looses * 100) / cfg::NUMBER_OF_REPITITIONS));
}

}