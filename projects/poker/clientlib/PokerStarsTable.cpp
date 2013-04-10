#include "PokerStarsTable.h"
#include "Messages.h"
#include "PokerStars.h"
#include "Modules.h"
#include "packet.pb.h"
#include "Logic.h"

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
const float FOLD_X = 1.7864864864864864864864864864865f;
const float FOLD_Y = 1.0579374275782155272305909617613f;
const float CHECK_CALL_X = 1.3628865979381443298969072164948f;
const float CHECK_CALL_Y = FOLD_Y;
const float BET_X = 1.1016666666666666666666666666667f;
const float BET_Y = FOLD_Y;

static boost::random::random_device g_Random;

template<typename T, typename F, typename A>
void Register(F& factory, A& arg)
{
	SCOPED_LOG(arg);

	T message(arg);
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

Table::Table(ILog& logger, HWND window, const net::IConnection::Ptr& connection) 
	: m_Log(logger)
	, m_Window(window)
	, m_Phase(Phase::Preflop)
	, m_Evaluator(new pcmn::Evaluator())
	, m_Actions(4)
	, m_Connection(connection)
	, m_IsNeedDecision(false)
	, m_ActionsParser(m_Log, m_Actions, m_Players)
	, m_IsCardsShowed(false)
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
	LOG_TRACE("Player: '%s', action: '%s', amount: '%s'") % name % pcmn::Action::ToString(action) % amount;
	static const std::string& botName = pcmn::Player::ThisPlayer().Name();

	if (action == pcmn::Action::ShowCards)
	{
		if (!m_IsCardsShowed)
			m_LastStacks = m_Stacks;

		m_IsCardsShowed = true;
		m_WaitingPlayers[name] = false;	
		return;
	}

	if (action == pcmn::Action::BigBlind)
	{
		if (!m_Actions[m_Phase].empty() && m_Actions[m_Phase].back().m_Value == pcmn::Action::BigBlind)
			return; // duplicated
	}

	if (action == pcmn::Action::SmallBlind || action == pcmn::Action::MoneyReturn)
	{
		// remove loosers
		const pcmn::Player::List::const_iterator it = std::remove_if
		(
			m_Players.begin(),
			m_Players.end(),
			[&](const pcmn::Player::Ptr& player)
			{
				if (player->Name() == name)
					return false;
				return !m_Stacks[player->Name()];
			}
		);

		m_Players.erase(it, m_Players.end());


		SendStatistic();
		ResetPhase();
	}

	if (action == pcmn::Action::Rank)
	{
		m_Loosers.push_back(name);
		if (amount < 3 || name == botName)
		{
			// game finished
			SendStatistic();
			CloseTableWindow(); 
			ResetPhase();
			return;
		}
	}	

	switch (action)
	{
	case pcmn::Action::Bet: 
	case pcmn::Action::Raise:
		{
			const unsigned difference = amount - m_Bets[name];
			m_Bets[name] += difference;
			m_TotalBets[name] += difference;
			m_WaitingPlayers.clear();
			m_WaitingPlayers[name] = true;
			break;
		}
	case pcmn::Action::Call: 
		m_WaitingPlayers[name] = true;
	case pcmn::Action::SmallBlind: 
	case pcmn::Action::BigBlind:
		m_TotalBets[name] += amount;
		m_Bets[name] += amount;
		break;
	case pcmn::Action::Check:
		m_WaitingPlayers[name] = true;
		break;
	default:
		break;
	}

	if (action == pcmn::Action::SecondsLeft && botName == name)
		OnBotAction(); // our turn to play

	m_Actions[m_Phase].push_back(ActionDesc(name, action, amount));

	if (action == pcmn::Action::SmallBlind)
	{
		pcmn::Player::Ptr currentPlayer = GetPlayer(name);
		if (!currentPlayer)
			return;

		currentPlayer = currentPlayer->GetNext();
		if (currentPlayer)
			PlayerAction(currentPlayer->Name(), pcmn::Action::BigBlind, amount * 2);
	}

	if (action == pcmn::Action::Fold)
		m_FoldedPlayers[name] = true;

	if (m_FoldedPlayers[botName] || m_WaitingPlayers[botName])
		return;

	if (action == pcmn::Action::SecondsLeft || action == pcmn::Action::Ante || action == pcmn::Action::ShowCards)
		return; // no need to check next player

	MakeDecisionIfNext(name);
}

void Table::FlopCards(const pcmn::Card::List& cards)
{
	m_Bets.clear();
	m_WaitingPlayers.clear();
	Phase::Value phase = Phase::Preflop;
	switch (cards.size())
	{
	case 3: 
		phase = Phase::Flop; 
        m_ActionsParser.Parse(false, m_Button); 
		break;
	case 4: 
		phase = Phase::Turn; 
		break;
	case 5: 
		phase = Phase::River; 
		break;
	default: assert(false);
	}

	m_FlopCards = cards;
	SetPhase(phase);
	MakeDecisionIfNext(m_Button);
}

void Table::BotCards(const pcmn::Card& first, const pcmn::Card& second)
{
	m_PlayerCards[pcmn::Player::ThisPlayer().Name()] = boost::assign::list_of(first)(second);

	if (m_IsNeedDecision)
		OnBotAction();
}

void Table::PlayersInfo(const pcmn::Player::List& players)
{
	for (const pcmn::Player::Ptr& player : players)
		m_Stacks[player->Name()] = player->Stack();
}

pcmn::Player::Ptr Table::GetPlayer(const std::string& name) 
{
	const pcmn::Player::List::iterator it = std::find_if
	(
		m_Players.begin(),
		m_Players.end(),
		[&](const pcmn::Player::Ptr& player)
		{
			return player ? player->Name() == name : false;
		}
	);

	if (it != m_Players.end())
		return *it;

	return pcmn::Player::Ptr();
}

void Table::OnBotAction()
{
	// make a decision and react
	m_IsNeedDecision = true;

	if (!m_PlayerCards.count(pcmn::Player::ThisPlayer().Name()))
		return; // wait for bot cards

	SendStatistic();

	m_Connection->Receive(boost::bind(&Table::ReceiveFromServerCallback, this, _1), &net::Reply());
}

void Table::PlayerCards(const std::string& name, const pcmn::Card::List& cards)
{
	m_PlayerCards[name] = cards;
}

void Table::ResetPhase()
{
	for (const std::string& name : m_Loosers)
		ErasePlayer(name);
	m_Loosers.clear();

	m_Phase = Phase::Preflop;
	m_FlopCards.clear();
	m_Actions.clear();
	m_Actions.resize(4);
	m_PlayerCards.clear();
	m_IsNeedDecision = false;
	m_Bets.clear();
	m_Loosers.clear();
	m_FoldedPlayers.clear();
	m_TotalBets.clear();
	m_Button.clear();
	m_WaitingPlayers.clear();
	m_IsCardsShowed = false;
}

void Table::SetPhase(const Phase::Value phase)
{
	for (const pcmn::Player::Ptr& p : m_Players)
	{
		p->Bet(0);
		if (p->State() != pcmn::Player::State::Fold && p->State() != pcmn::Player::State::AllIn)
			p->State(pcmn::Player::State::Waiting);
	}

	m_Phase = phase;
}

void Table::SendStatistic()
{
	if (!m_ActionsParser.Parse(m_IsNeedDecision, m_Button))
		return;

	assert(!m_Button.empty());

	m_IsNeedDecision = false;

	net::Packet packet;

	net::Packet::Table& table = *packet.mutable_info();

	for (const pcmn::Card& card : m_FlopCards)
		table.mutable_cards()->Add(card.ToEvalFormat());	

	std::size_t counter = 0;
	std::map<std::string, std::size_t> players;
	for (const pcmn::Player::Ptr& player : m_Players)
	{
		net::Packet::Player& added = *table.add_players();
		added.set_bet(player->Bet());
		added.set_name(player->Name());

		const unsigned stack = (m_IsCardsShowed ? m_LastStacks[player->Name()] : m_Stacks[player->Name()]) + m_TotalBets[player->Name()];
		added.set_stack(stack);

		const pcmn::Card::List& cards = m_PlayerCards[player->Name()];
		for (const pcmn::Card& card : cards)
			added.mutable_cards()->Add(card.ToEvalFormat());

		if (player->Name() == m_Button)
			table.set_button(counter);

		players[player->Name()] = counter;
		++counter;
	}

	for (const Actions& actions : m_Actions)
	{
		if (actions.empty())
			break;

		net::Packet::Phase& phase = *packet.add_phases();
		for (const ActionDesc& dsc : actions)
		{
			net::Packet::Action& action = *phase.add_actions();
			action.set_amount(dsc.m_Amount);
			action.set_id(dsc.m_Value);
			action.set_player(players[dsc.m_Name]);
		}
	}

	try
	{
		m_Connection->Send(packet);
	}
	catch (const std::exception& e)
	{
		LOG_WARNING("Failed to send statistics, error: [%s]") % e.what();
	}

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
		m_FoldedPlayers[pcmn::Player::ThisPlayer().Name()] = true;
		Fold();
		break;
	case pcmn::Action::Check:
	case pcmn::Action::Call:
		CheckCall();
		break;
	case pcmn::Action::Bet:
	case pcmn::Action::Raise:
		BetRaise(reply.amount());
		break;
	default:
		assert(false);
	}
}

void Table::PressButton(const float x, const float y)
{
	boost::thread(boost::bind(&Table::PressButtonThread, this, x, y));
}

void Table::PressButtonThread(const float x, const float y)
{
	SCOPED_LOG(m_Log);

	boost::random::uniform_int_distribution<> wait(1000, 2000);

	boost::this_thread::interruptible_wait(wait(g_Random));

	RECT rect;
	CHECK(GetWindowRect(m_Window, &rect));

	const float resultX = static_cast<float>(rect.right - rect.left) / x;
	const float resultY = static_cast<float>(rect.bottom - rect.top) / y;

	LOG_TRACE("Pressing to: x='%s', y='%s'") % resultX % resultY;

	const LPARAM param = MAKELPARAM(resultX, resultY);
	CHECK(PostMessage(m_Window, WM_LBUTTONDOWN, (WPARAM)MK_LBUTTON, param));

	boost::random::uniform_int_distribution<> up(100, 200);

	boost::this_thread::interruptible_wait(up(g_Random));
	CHECK(PostMessage(m_Window, WM_LBUTTONUP, NULL, param));
}

void Table::Fold()
{
	PressButton(FOLD_X, FOLD_Y);
}

void Table::CheckCall()
{
	PressButton(CHECK_CALL_X, CHECK_CALL_Y);
	boost::this_thread::interruptible_wait(3000);
	PressButton(BET_X, BET_Y); // in case of all in call
}

void Table::BetRaise(unsigned amount)
{
	const HWND slider = FindWindowEx(m_Window, NULL, "PokerStarsSliderClass", NULL);	
	CHECK(slider != NULL, "Failed to find slider");
	const HWND editor = FindWindowEx(slider, NULL, "PokerStarsSliderEditorClass", NULL);
	CHECK(editor != NULL, "Failed to find editor");

	const std::string value = boost::lexical_cast<std::string>(amount);
	SendMessage(editor, WM_SETTEXT, NULL, reinterpret_cast<LPARAM>(value.c_str()));

	PressButton(BET_X, BET_Y);
}

void Table::ErasePlayer(const std::string& name)
{
	const pcmn::Player::List::iterator it = std::find_if
	(
		m_Players.begin(),
		m_Players.end(),
		[&](const pcmn::Player::Ptr& player)
		{
			return player ? player->Name() == name : false;
		}
	);

	if (it == m_Players.end())
		return;

	(*it)->DeleteLinks();
	m_Players.erase(it);
	m_Stacks.erase(name);
}

void Table::MakeDecisionIfNext(const std::string& current)
{
	pcmn::Player::Ptr currentPlayer = GetPlayer(current);
	if (!currentPlayer)
		return;

	for (;;)
	{
		currentPlayer = currentPlayer->GetNext();
		if (!currentPlayer)
			break;

		if (m_FoldedPlayers[currentPlayer->Name()])
			continue;

		if (currentPlayer->Name() == pcmn::Player::ThisPlayer().Name())
			OnBotAction(); // our turn to play

		break;
	}
}

} // namespace ps
} // namespace clnt




