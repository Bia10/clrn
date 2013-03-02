#ifndef Config_h__
#define Config_h__

namespace cfg
{
	// evaluator
	static const int MAX_EQUITY_PLAYERS = 4;
	static const int CARD_DECK_SIZE = 52;
	static const int MAX_PLAYERS = 9;
	static const int NUMBER_OF_REPITITIONS = 
		1000 * 100
#ifndef _DEBUG
		* 
		10
#endif
		;
	
	// neuro
	static const int LAYERS_COUNT = 3;
	static const int HIDDEN_NEIRONS_COUNT = 3;
	static const int INPUT_COUNT = 9;
	static const int OUTPUT_COUNT = 3;
};

#endif // Config_h__
