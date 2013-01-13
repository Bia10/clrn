#include "PokerStarsTable.h"
#include "Messages.h"
#include "PokerStars.h"
#include "Modules.h"

#include <windows.h>

#include <boost/functional/factory.hpp>
#include <boost/bind.hpp>

namespace clnt
{
namespace ps
{

Table::Factory Table::s_Factory;
const std::size_t CURRENT_MODULE_ID = Modules::Table;

template<typename T, typename F, typename A>
void Register(F& factory, A& arg)
{
	T message(arg);
	const std::size_t id = static_cast<const IMessage&>(message).GetId();
	factory.Register(id, boost::bind(boost::factory<T*>(), boost::ref(arg)));
}


Table::Table(ILog& logger) : m_Log(logger)
{
	if (s_Factory.IsEmpty())
	{
		Register<msg::PlayerAction>(s_Factory, m_Log);
	}
}

void Table::HandleMessage(const dasm::WindowMessage& message)
{
	const std::size_t messageId = _byteswap_ulong(*reinterpret_cast<const std::size_t*>(message.m_Block.m_Data + message.m_Block.m_Offset + 0xd));
	const std::auto_ptr<IMessage> handler(s_Factory.Create(messageId));

	if (!handler.get())
		LOG_DEBUG("Message [%s] ignored") % messageId;
	else
		handler->Process(message, *this);
}

void Table::PlayerAction(const std::string& name, Action::Value, std::size_t amount)
{

}

void Table::FlopCards(const std::vector<Card>& cards)
{

}

void Table::PlayerCards(const Card& first, const Card& second)
{

}

void Table::PlayersInfo(const Player::List& players)
{

}

} // namespace ps
} // namespace clnt