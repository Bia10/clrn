#include "SevenEval.h"
#include "FiveEval.h"

#include <stdio.h>

SevenEval::SevenEval() {
    int face[13] = {ACE, KING, QUEEN, JACK, TEN, NINE, EIGHT, SEVEN, SIX, FIVE, FOUR, THREE, TWO };
    int faceFlush[13] = {ACE_FLUSH, KING_FLUSH, QUEEN_FLUSH, JACK_FLUSH, TEN_FLUSH, NINE_FLUSH, EIGHT_FLUSH, SEVEN_FLUSH, SIX_FLUSH, FIVE_FLUSH, FOUR_FLUSH,
                         THREE_FLUSH, TWO_FLUSH
                        };
                        
    for ( int n = 0; n < 13; n++ ) {
        deckcardsKey[4 * n] = ( face[n] << NON_FLUSH_BIT_SHIFT ) + SPADE;
        deckcardsKey[4 * n + 1] = ( face[n] << NON_FLUSH_BIT_SHIFT ) + HEART;
        deckcardsKey[4 * n + 2]   = ( face[n] << NON_FLUSH_BIT_SHIFT ) + DIAMOND;
        deckcardsKey[4 * n + 3]   = ( face[n] << NON_FLUSH_BIT_SHIFT ) + CLUB;
        deckcardsFlush[4 * n] = faceFlush[n];
        deckcardsFlush[4 * n + 1] = faceFlush[n];
        deckcardsFlush[4 * n + 2] = faceFlush[n];
        deckcardsFlush[4 * n + 3] = faceFlush[n];
        deckcardsSuit[4 * n]      = SPADE;
        deckcardsSuit[4 * n + 1]  = HEART;
        deckcardsSuit[4 * n + 2]  = DIAMOND;
        deckcardsSuit[4 * n + 3]  = CLUB;
    }
    
    int count = 0;
    
    // Erase all ranks and flushranks
    for ( int i = 0; i < MAX_NONFLUSH_KEY_INT + 1; i++ ) {
        rankArray[i % CIRCUMFERENCE_SEVEN] = 0;
    }
    
    for ( int i = 0; i < MAX_SEVEN_FLUSH_KEY_INT + 1; i++ ) {
        flushRankArray[i] = 0;
    }
    
    // Generate seven-ranks from five-ranks
    FiveEval *fiveCardEvaluator = new FiveEval();
    
    // Non-flush ranks
    for ( int i = 1; i < 13; i++ ) {
        for ( int j = 1; j <= i; j++ ) {
            for ( int k = 1; k <= j; k++ ) {
                for ( int l = 0; l <= k; l++ ) {
                    for ( int m = 0; m <= l; m++ ) {
                        for ( int n = 0; n <= m; n++ ) {
                            for ( int p = 0; p <= n; p++ ) {
                                if ( i != m && j != n && k != p ) {
                                    int key = face[i] + face[j] + face[k] + face[l] + face[m] + face[n] + face[p];
                                    // The 4*i+0 and 4*m+1 trick prevents flushes
                                    short rank = fiveCardEvaluator->getRankOfSeven ( 4 * i, 4 * j, 4 * k, 4 * l, 4 * m + 1, 4 * n + 1, 4 * p + 1 );
                                    rankArray[key < CIRCUMFERENCE_SEVEN ? key : key - CIRCUMFERENCE_SEVEN] = rank;
                                    count++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Flush ranks
    // All 7 same suit:
    for ( int i = 6; i < 13; i++ ) {
        for ( int j = 5; j < i; j++ ) {
            for ( int k = 4; k < j; k++ ) {
                for ( int l = 3; l < k; l++ ) {
                    for ( int m = 2; m < l; m++ ) {
                        for ( int n = 1; n < m; n++ ) {
                            for ( int p = 0; p < n; p++ ) {
                                int key = faceFlush[i] + faceFlush[j] + faceFlush[k] + faceFlush[l] + faceFlush[m] + faceFlush[n] + faceFlush[p];
                                short rank = fiveCardEvaluator->getRankOfSeven ( 4 * i, 4 * j, 4 * k, 4 * l, 4 * m, 4 * n, 4 * p );
                                flushRankArray[key] = rank;
                                count++;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Only 6 same suit:
    for ( int i = 5; i < 13; i++ ) {
        for ( int j = 4; j < i; j++ ) {
            for ( int k = 3; k < j; k++ ) {
                for ( int l = 2; l < k; l++ ) {
                    for ( int m = 1; m < l; m++ ) {
                        for ( int n = 0; n < m; n++ ) {
                            int key = faceFlush[i] + faceFlush[j] + faceFlush[k] + faceFlush[l] + faceFlush[m] + faceFlush[n];
                            // The Two of clubs is the card at index 51; the other six cards all have the spade suit.
                            short rank = fiveCardEvaluator->getRankOfSeven ( 4 * i, 4 * j, 4 * k, 4 * l, 4 * m, 4 * n, 51 );
                            flushRankArray[key] = rank;
                            count++;
                        }
                    }
                }
            }
        }
    }
    
    // Only 5 same suit:
    for ( int i = 4; i < 13; i++ ) {
        for ( int j = 3; j < i; j++ ) {
            for ( int k = 2; k < j; k++ ) {
                for ( int l = 1; l < k; l++ ) {
                    for ( int m = 0; m < l; m++ ) {
                        int key = faceFlush[i] + faceFlush[j] + faceFlush[k] + faceFlush[l] + faceFlush[m];
                        short rank = fiveCardEvaluator->getRankOfFive ( 4 * i, 4 * j, 4 * k, 4 * l, 4 * m );
                        flushRankArray[key] = rank;
                        count++;
                    }
                }
            }
        }
    }
    
    // Initialise flush checks.
    short SUIT_COUNT = 0, FLUSH_SUIT_INDEX = -1, CARDS_MATCHED_SO_FAR = 0;
    short SUIT_KEY = SPADE;
    short suits[4] = {SPADE, HEART, DIAMOND, CLUB};
    
    // Initialise all entries of flushCheck[] to UNVERIFIED, as yet unchecked.
    for ( int i = 0 ; i < MAX_FLUSH_CHECK_SUM + 1 ; i++ ) {
        flushCheck[i] = UNVERIFIED;
    }
    
    // 7-card flush
    for ( int card_1 = 0; card_1 < NUMBER_OF_SUITS; card_1++ ) {
        for ( int card_2 = 0; card_2 <= card_1; card_2++ ) {
            for ( int card_3 = 0; card_3 <= card_2; card_3++ ) {
                for ( int card_4 = 0; card_4 <= card_3; card_4++ ) {
                    for ( int card_5 = 0; card_5 <= card_4; card_5++ ) {
                        for ( int card_6 = 0; card_6 <= card_5; card_6++ ) {
                            for ( int card_7 = 0; card_7 <= card_6; card_7++ ) {
                                SUIT_COUNT = 0, FLUSH_SUIT_INDEX = -1, CARDS_MATCHED_SO_FAR = 0;
                                SUIT_KEY = suits[card_1] + suits[card_2] + suits[card_3] + suits[card_4] + suits[card_5] + suits[card_6] + suits[card_7];
                                
                                if ( flushCheck[SUIT_KEY] == UNVERIFIED ) {
                                    do {
                                        FLUSH_SUIT_INDEX++;
                                        SUIT_COUNT = ( suits[card_1] == suits[FLUSH_SUIT_INDEX] ) +
                                                     ( suits[card_2] == suits[FLUSH_SUIT_INDEX] ) +
                                                     ( suits[card_3] == suits[FLUSH_SUIT_INDEX] ) +
                                                     ( suits[card_4] == suits[FLUSH_SUIT_INDEX] ) +
                                                     ( suits[card_5] == suits[FLUSH_SUIT_INDEX] ) +
                                                     ( suits[card_6] == suits[FLUSH_SUIT_INDEX] ) +
                                                     ( suits[card_7] == suits[FLUSH_SUIT_INDEX] );
                                        CARDS_MATCHED_SO_FAR += SUIT_COUNT;
                                    }
                                    while ( CARDS_MATCHED_SO_FAR < 3 && FLUSH_SUIT_INDEX < 4 );
                                    
                                    // A count of 5 or more means we have a flush. We place the value of the flush suit here.
                                    if ( SUIT_COUNT > 4 ) {
                                        flushCheck[SUIT_KEY] = suits[FLUSH_SUIT_INDEX];
                                    }
                                    
                                    else {   // Otherwise this is a non-flush hand.
                                        flushCheck[SUIT_KEY] = NOT_A_FLUSH;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


SevenEval::~SevenEval() {}

short SevenEval::getRankOfSeven ( const int CARD1, const int CARD2, const int CARD3, const int CARD4, const int CARD5, const int CARD6, const int CARD7 ) const
{
    // Create a 7-card hand key by adding up each of the card keys.
    unsigned long KEY = deckcardsKey[CARD1] + deckcardsKey[CARD2] + deckcardsKey[CARD3] + deckcardsKey[CARD4] + deckcardsKey[CARD5] + deckcardsKey[CARD6] +
                        deckcardsKey[CARD7];
    int FLUSH_CHECK_KEY = ( KEY & SUIT_BIT_MASK ); // Tear off the flush check strip.
    short FLUSH_SUIT = flushCheck[FLUSH_CHECK_KEY];
    
    if ( FLUSH_SUIT == NOT_A_FLUSH ) {
        // Tear off the non-flush key strip, and look up the rank.
        KEY = ( KEY >> NON_FLUSH_BIT_SHIFT );
        // Take key modulo the circumference. A dichotomy is faster than using the usual modulus operation. This is fine for us because the is more than half the
        // largest face key we come across.
        return rankArray[ ( KEY < CIRCUMFERENCE_SEVEN ? KEY : KEY - CIRCUMFERENCE_SEVEN )];
    }
    
    else {
        // Generate a flush key, and look up the rank.
        int FLUSH_KEY = ( deckcardsSuit[CARD1] == FLUSH_SUIT ? deckcardsFlush[CARD1] : 0 ) +
                        ( deckcardsSuit[CARD2] == FLUSH_SUIT ? deckcardsFlush[CARD2] : 0 ) +
                        ( deckcardsSuit[CARD3] == FLUSH_SUIT ? deckcardsFlush[CARD3] : 0 ) +
                        ( deckcardsSuit[CARD4] == FLUSH_SUIT ? deckcardsFlush[CARD4] : 0 ) +
                        ( deckcardsSuit[CARD5] == FLUSH_SUIT ? deckcardsFlush[CARD5] : 0 ) +
                        ( deckcardsSuit[CARD6] == FLUSH_SUIT ? deckcardsFlush[CARD6] : 0 ) +
                        ( deckcardsSuit[CARD7] == FLUSH_SUIT ? deckcardsFlush[CARD7] : 0 );
        return flushRankArray[FLUSH_KEY];
    }
    
    return -1;
}


// Check that FiveEval and SevenEval return the same ranks for all seven card hands. This does not prove that SevenEval is correct, unless FiveEval is correct.
void SevenEval::validateAgainstFiveEval() {
    FiveEval *fEval = new FiveEval();
    SevenEval *sEval = new SevenEval();
    
    for ( int i = 6; i < 52; i++ ) {
        for ( int j = 5; j < i; j++ ) {
            printf ( "\n%i__%i", i, j );
            
            for ( int k = 4; k < j; k++ ) {
                for ( int l = 3; l < k; l++ ) {
                    for ( int m = 2; m < l; m++ ) {
                        for ( int n = 1; n < m; n++ ) {
                            for ( int p = 0; p < n; p++ ) {
                                int rank5 = fEval->getRankOfSeven ( i, j, k, l, m, n, p );
                                int rank7 = sEval->getRankOfSeven ( i, j, k, l, m, n, p );
                                
                                if ( rank5 != rank7 ) {
                                    printf ( "\nMismatch for %i %i %i %i %i %i %i", i, j , k, l, m , n, p );
                                    return;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    printf ( "\nTest passed, FiveEval and SevenEval return the same ranks everywhere" );
    return;
}