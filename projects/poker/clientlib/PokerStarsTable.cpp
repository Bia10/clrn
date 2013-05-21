#include "PokerStarsTable.h"
#include "Messages.h"
#include "PokerStars.h"
#include "Modules.h"
#include "packet.pb.h"
#include "TableLogic.h"

#include <windows.h>

#include <boost/functional/factory.hpp>
#include <boost/bind.hpp>
#include <boost/assign.hpp>
#include <boost/thread.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/random_device.hpp>
#include <boost/make_shared.hpp>

namespace clnt
{
namespace ps
{

Table::Factory Table::s_Factory;
const std::size_t CURRENT_MODULE_ID = Modules::Table;

template<typename T, typename F, typename A>
void Register(F& factory, A& arg)
{
	SCOPED_LOG(arg);

	const T message(arg);
	const std::size_t id = static_cast<const IMessage&>(message).GetId();
	factory.Register(id, boost::bind(boost::factory<T*>(), boost::ref(arg)));
}

template<typename Stream>
void BytesToString(Stream& stream, const void* data, std::size_t size)
{
	for (std::size_t i = 0 ; i < size; ++i)
	{
		const unsigned char symbol = reinterpret_cast<const unsigned char*>(data)[i];
		stream 
			<< std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(symbol)
			<< std::setw(0) << "'" << std::dec;

		if (symbol < 0x20)
			stream << " ";
		else
			stream << symbol;
		stream << "' ";
	}
}

Table::Table(ILog& logger, HWND window, const net::IConnection::Ptr& connection, const pcmn::Evaluator& evaluator, ITableControl* ctrl) 
	: m_Log(logger)
	, m_Window(window)
	, m_Evaluator(evaluator)
	, m_Connection(connection)
    , m_Logic(m_Log, *this, m_Evaluator)
    , m_Control(ctrl)
{
	SCOPED_LOG(m_Log);

	if (s_Factory.IsEmpty())
	{
		Register<msg::PlayerAction>(s_Factory, m_Log);
		Register<msg::FlopCards>(s_Factory, m_Log);
		Register<msg::TurnAndRiverCards>(s_Factory, m_Log);
		Register<msg::PlayerCards>(s_Factory, m_Log);
		Register<msg::CashGameInfo>(s_Factory, m_Log);
		Register<msg::BeforePreflopActions>(s_Factory, m_Log);
		Register<msg::TableInfo>(s_Factory, m_Log);
	}
}

void Table::HandleMessage(const dasm::WindowMessage& message)
{
	TRY 
	{
		SCOPED_LOG(m_Log);

		if (!message.m_Block.m_Data)
			return;

		const std::size_t messageId = _byteswap_ulong(*reinterpret_cast<const std::size_t*>(message.m_Block.m_Data + message.m_Block.m_Offset + 0xd));
		const std::size_t commandId = _byteswap_ulong(*reinterpret_cast<const std::size_t*>(message.m_Block.m_Data + message.m_Block.m_Offset + 0x9));
		const std::auto_ptr<IMessage> handler(s_Factory.Create(commandId ? messageId : commandId));

		if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Debug))
		{
			std::ostringstream oss;
			BytesToString(oss, message.m_Block.m_Data + message.m_Block.m_Offset, message.m_Block.m_Offset + message.m_Block.m_Size);
			LOG_DEBUG("%s: [0x%x], data: [%s]") % (handler.get() ? "Processing" : "Ignoring") % messageId % oss.str();
		}
		
		if (handler.get())
			handler->Process(message, *this);
	}
	catch (CExcept& e)
	{
		std::vector<char> data;
		std::ostringstream oss;
		BytesToString(oss, message.m_Block.m_Data + message.m_Block.m_Offset, message.m_Block.m_Offset + message.m_Block.m_Size);

		e.Append(__FILE__, __LINE__);
		APPEND_ARGS(e, "Failed to handle message by table", oss.str()); 
		throw;	
	}
}

void Table::PlayerAction(const std::string& name, const pcmn::Action::Value action, const std::size_t amount)
{
    SCOPED_LOG(m_Log);

	LOG_TRACE("Player: '%s', action: '%s', amount: '%s'") % name % pcmn::Action::ToString(action) % amount;

    m_Logic.PushAction(name, action, amount);

    static const std::string& botName = pcmn::Player::ThisPlayer().Name();
    if (action == pcmn::Action::SecondsLeft && botName == name)
        m_Logic.SendRequest(false); // our turn to play
}

void Table::FlopCards(const pcmn::Card::List& cards)
{
    SCOPED_LOG(m_Log);

    m_Logic.SetFlopCards(cards);
    if (m_Logic.IsRoundCompleted() || m_Logic.IsValid())
        return;

    switch (cards.size())
    {
    case 3: 
        m_Logic.SetPhase(pcmn::TableLogic::Phase::Flop);
        break;
    case 4: 
        m_Logic.SetPhase(pcmn::TableLogic::Phase::Turn);
        break;
    case 5: 
        m_Logic.SetPhase(pcmn::TableLogic::Phase::River);
        break;
    default: assert(false);
    }
}

void Table::BotCards(const pcmn::Card& first, const pcmn::Card& second)
{
    SCOPED_LOG(m_Log);

    if (m_Logic.IsRoundCompleted())
    {
        // this info from next round
        pcmn::Player::List data = boost::assign::list_of(pcmn::Player(pcmn::Player::ThisPlayer().Name(), 0));
        data.back().Cards(boost::assign::list_of(first)(second));
        m_Logic.SetNextRoundData(data);
        return;
    }

    m_Logic.SetPlayerCards(pcmn::Player::ThisPlayer().Name(), boost::assign::list_of(first)(second));
}

void Table::PlayersInfo(const pcmn::Player::List& players)
{
    SCOPED_LOG(m_Log);

    if (m_Logic.IsRoundCompleted())
    {
        // this info from next round
        m_Logic.SetNextRoundData(players);
        return;
    }

    for (const pcmn::Player& player : players)
        m_Logic.SetPlayerStack(player.Name(), player.Stack());
}

void Table::PlayerCards(const std::string& name, const pcmn::Card::List& cards)
{
    SCOPED_LOG(m_Log);

    m_Logic.SetPlayerCards(name, cards);
}

void Table::CloseTableWindow()
{
	CHECK(PostMessage(m_Window, WM_SYSCOMMAND, SC_CLOSE, 0), GetLastError());
}

void Table::ReceiveFromServerCallback(const google::protobuf::Message& message)
{
	LOG_TRACE("Received decision from server: [%s]") % message.ShortDebugString();

	const net::Reply& reply = dynamic_cast<const net::Reply&>(message);

	const pcmn::Action::Value action = reply.error().empty() ? static_cast<pcmn::Action::Value>(reply.action()) : pcmn::Action::Fold;

	switch (action)
	{
	case pcmn::Action::Fold:
		m_Control->Fold();
		break;
	case pcmn::Action::Check:
	case pcmn::Action::Call:
		m_Control->CheckCall();
		break;
	case pcmn::Action::Bet:
	case pcmn::Action::Raise:
		m_Control->BetRaise(reply.amount());
		break;
	default:
		assert(false);
	}
}

void Table::SendRequest(const net::Packet& packet, bool statistics)
{
    try
    {
        LOG_TRACE("Packet: [%s]") % packet.DebugString();
        m_Connection->Send(packet);

        if (!statistics)
            m_Connection->Receive(boost::bind(&Table::ReceiveFromServerCallback, this, _1), &net::Reply());
    }
    catch (const std::exception& e)
    {
        LOG_WARNING("Failed to send statistics, error: [%s]") % e.what();
        if (!statistics)
            m_Control->Fold();
    }
}

void Table::MakeDecision(const pcmn::Player& player, const pcmn::Player::Queue& activePlayers, const pcmn::TableContext& context, const pcmn::Player::Position::Value position)
{
    throw std::exception("The method or operation is not implemented.");
}

void Table::WriteStatistics(pcmn::TableContext::Data& data)
{
    throw std::exception("The method or operation is not implemented.");
}

} // namespace ps
} // namespace clnt




