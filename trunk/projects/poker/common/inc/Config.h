#ifndef Config_h__
#define Config_h__

namespace cfg
{
	// evaluator
    static const int MAX_EQUITY_PLAYERS = 4;
    static const int MIN_EQUITY_PLAYERS = 2; 
	static const int CARD_DECK_SIZE = 52;
	static const int MAX_PLAYERS = 9;
	static const int NUMBER_OF_REPITITIONS = 
		1000 * 100
#ifndef _DEBUG
		* 
		10
#endif
		;
    static const int MIN_CARD_RANGE = 15;
	
	// neuro
	static const int LAYERS_COUNT = 3;
	static const int HIDDEN_NEIRONS_COUNT = 18;
	static const int INPUT_COUNT = 8;
	static const int OUTPUT_COUNT = 3;
	static const char NETWORK_FILE_NAME[] = "neuro.db";
	static const int TEACH_REPETITIONS_COUNT = 50000;

	// network
	static const int DEFAULT_PORT = 5000;

	// server
	static const int THREADS_COUNT = 4;
	static const char DB_FILE_NAME[] = "stat.db"; 

	// teacher
	static const char DATA_FILE_NAME[] = "train_data.txt";

    // logic
    static const int DEFAULT_STACK = 1500;
};

#endif // Config_h__
