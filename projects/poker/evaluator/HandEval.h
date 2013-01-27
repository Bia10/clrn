#ifndef HANDEVAL_H
#define HANDEVAL_H

#include "Constants.h"
#include <vector>

  // Uncomment when testing. Make this number is
  // very large, to allow the evaluator to get up
  // to speed, and divisible by 7. Also close any
  // other applications that are open.
  #define BIG_NUMBER 30000000*7

class HandEval {
 private:
  
    // Uncomment when timing.
    //char hands_array[BIG_NUMBER];
  
    /*!
     Ranks for 7-card evaluation separated
     into non-flushes and flushes, each with
     their own respective keys
     */
	unsigned short rankArray[CIRCUMFERENCE_SEVEN];
	unsigned short flushRankArray[MAX_SEVEN_FLUSH_KEY_INT+1];
  
    /*!
     Card face values beginning with ACE_ from
     index 0 and TWO_ from index 48.
     */
	unsigned int deckcardsKey[DECK_SIZE];
	unsigned short deckcardsFlush[DECK_SIZE];
	unsigned short deckcardsSuit[DECK_SIZE];	
  
    /*!
     Array comprising of the flush suits.
     */
	short flushCheck[MAX_FLUSH_CHECK_SUM + 1];
  
    /*!
     Generators, called during construction.
     */
  void generateDeckKeys();
  void generateRanks();
  void generateFlushCheck();
  
    /*!
     Ranking method.
     */
  short getRankOfSeven(const int card_1,
                       const int card_2,
                       const int card_3,
                       const int card_4,
                       const int card_5,
                       const int card_6,
                       const int card_7);
  
public:
  /*!
   Constructor.
   */
  HandEval();
  
  /*!
   Destructor.
   */
  ~HandEval();
  
  /*!
   Equity evalaution methods.
   */
  double computePreFlopEquityForSpecificHoleCards (const int * holeCards,
                                                         int number_of_players);
  
  double computeFlopEquityForSpecificCards (const int * holeCards,
                                                  const int * tableCards,
                                                  int number_of_players);
  
  double computeTurnEquityForSpecificCards (const int * hole_cards,
                                                 const int * table_cards,
                                                  int number_of_players);
  
  double computeRiverEquityForSpecificCards (const int * hole_cards,
                                                   const int * table_cards,
                                                   int number_of_players);
  
  void EvaluateHands(const int* hands, const unsigned int size, short* results);

  // Uncomment when timing the performance.
  void timeRankMethod();
};

#endif
