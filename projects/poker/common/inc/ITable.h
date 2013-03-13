#ifndef ITable_h__
#define ITable_h__

#include "Actions.h"
#include "Cards.h"
#include "Player.h"

#include <windows.h>
#include <string>
#include <vector>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace dasm
{
	struct WindowMessage;
}

namespace clnt
{

//! Poker table abstraction
class ITable : boost::noncopyable
{
public:

	struct ActionDesc
	{
		ActionDesc(const std::string& name, pcmn::Action::Value value, std::size_t amount)
			: m_Name(name)
			, m_Value(value)
			, m_Amount(amount)
		{}
		std::string m_Name;
		pcmn::Action::Value m_Value;
		std::size_t m_Amount;
	};

	struct Phase
	{
		enum Value
		{
			Preflop	= 0,
			Flop = 1,
			Turn = 2,
			River = 3
		};
	};


	typedef boost::shared_ptr<ITable> Ptr;
	typedef std::vector<ActionDesc> Actions;
	typedef std::map<std::string, unsigned> StackMap;

	virtual ~ITable() {}

	virtual void HandleMessage(const dasm::WindowMessage& message) = 0;
	virtual void PlayerAction(const std::string& name, pcmn::Action::Value, std::size_t amount) = 0;
	virtual void FlopCards(const pcmn::Card::List& cards) = 0;
	virtual void BotCards(const pcmn::Card& first, const pcmn::Card& second) = 0;
	virtual void PlayerCards(const std::string& name, const pcmn::Card::List& cards) = 0;
	virtual void PlayersInfo(const pcmn::Player::List& players) = 0;
	virtual void Ante(std::size_t value) = 0;

};

}

#endif // ITable_h__
