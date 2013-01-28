#ifndef Parser_h__
#define Parser_h__

#include "ILog.h"
#include "packet.pb.h"

#include <vector>

namespace srv
{

//! Stats packet parser
class Parser
{
public:

	//! Packet parsed data
	struct Data
	{
		struct Player
		{
			typedef std::vector<Player> List;
			std::string m_Name;
			__int64 m_Index;
			int m_Position;		
			std::vector<float> m_Percents; // player percents on different streets
		};

		struct Action
		{
			typedef std::vector<Action> List;
			std::size_t m_PlayerIndex; // reference to one of the m_Players(index in the vector)
			int m_Street;
			int m_Action;
			float m_PotAmount;
			float m_StackAmount;
		};

		struct Hand
		{
			typedef std::vector<Hand> List;
			int m_First;
			int m_Second;
		};

		Player::List m_Players;
		Action::List m_Actions;
		Hand::List m_Hands;
	};

	Parser(ILog& logger);
	~Parser();
	bool Parse(const net::Packet& packet, Data& result); // returns true if next player bot and he need to do something

private:
	class Impl;
	Impl* m_Impl;

};

} // namespace srv

#endif // Parser_h__
