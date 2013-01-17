#ifndef HANDEVAL_H
#define HANDEVAL_H

#include "Constants.h"

  // Uncomment when testing. Make this number is
  // very large, to allow the evaluator to get up
  // to speed, and divisible by 7. Also close any
  // other applications that are open.
  #define BIG_NUMBER 3000000*7

class HandEval {
 private:
  
    // Uncomment when timing.
    char hands_array[BIG_NUMBER];
  
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
  const char * computePreFlopEquityForSpecificHoleCards (int * holeCards,
                                                         int number_of_players);
  
  const char * computeFlopEquityForSpecificCards (int * holeCards,
                                                  int * tableCards,
                                                  int number_of_players);
  
  const char * computeTurnEquityForSpecificCards (int * hole_cards,
                                                  int * table_cards,
                                                  int number_of_players);
  
  const char * computeRiverEquityForSpecificCards (int * hole_cards,
                                                   int * table_cards,
                                                   int number_of_players);
  
  // Uncomment when timing the performance.
  void timeRankMethod();
};

#endif
