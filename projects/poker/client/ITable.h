#ifndef ITable_h__
#define ITable_h__

#include "Actions.h"
#include "Cards.h"
#include "Player.h"

#include <windef.h>
#include <string>
#include <vector>

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

	typedef boost::shared_ptr<ITable> Ptr;

	virtual ~ITable() {}

	virtual void HandleMessage(const dasm::WindowMessage& message) = 0;
	virtual void PlayerAction(const std::string& name, Action::Value, std::size_t amount) = 0;
	virtual void FlopCards(const std::vector<Card>& cards) = 0;
	virtual void PlayerCards(const Card& first, const Card& second) = 0;
	virtual void PlayersInfo(const Player::List& players) = 0;

};

}

#endif // ITable_h__
