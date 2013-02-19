#ifndef Cards_h__
#define Cards_h__

#include <string>
#include <vector>

namespace pcmn
{
	struct Suit
	{
		enum Value
		{
			Unknown		= 0,
			Hearts		= 'h',
			Clubs		= 'c',
			Spades		= 's',
			Diamonds	= 'd'
		};

		static std::string ToString(Value value);
	};

	struct Card
	{
		typedef std::vector<Card> List;
		enum Value
		{
			Unknown	= 0,
			Two		= 2,
			Three	= 3,
			Four	= 4,
			Five	= 5,
			Six		= 6,
			Seven	= 7,
			Eight	= 8,
			Nine	= 9,
			Ten		= 10,
			Jack	= 11,
			Queen	= 12,
			King	= 13,
			Ace		= 14
		};

		Card() : m_Suit(Suit::Unknown), m_Value(Unknown) {}
		Card(Value value, Suit::Value suit) 
			: m_Value(value)
			, m_Suit(suit)
		{}

		static std::string ToString(Value value);
		static Value FromString(const char value);
		short ToEvalFormat() const;
		void FromEvalFormat(short value);

		Value m_Value;
		Suit::Value m_Suit;
	};

}

#endif // Cards_h__