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
	, m_Ante(0)
{
	SCOPED_LOG(m_Log);

	if (s_Factory.IsEmpty())
	{
		Register<msg::PlayerAction>(s_Factory, m_Log);
		Register<msg::FlopCards>(s_Factory, m_Log);
		Register<msg::TurnAndRiverCards>(s_Factory, m_Log);
		Register<msg::PlayerCards>(s_Factory, m_Log);
		Register<msg::PlayersInfo>(s_Factory, m_Log);
		Register<msg::PlayerInfo>(s_Factory, m_Log);
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

void Table::PlayerAction(const std::string& name, pcmn::Action::Value action, std::size_t amount)
{
	LOG_TRACE("Player: '%s', action: '%s', amount: '%s'") % name % pcmn::Action::ToString(action) % amount;
	
	const pcmn::Player::Ptr currentPlayer = GetPlayer(name);

// 	if (action == pcmn::Action::SmallBlind)
// 	{
// 		SendStatistic();
// 		ResetPhase();
// 		BetAnte();
// 	}
// 	else
	if (currentPlayer && action == pcmn::Action::Rank)
	{
		if (amount < 3 || currentPlayer->Name() == pcmn::Player::ThisPlayer().Name())
		{
			// game finished
			SendStatistic();
			CloseTableWindow(); 
		}
		RemovePlayer(name);
		return;
	}	

	m_Actions[m_Phase].push_back(ActionDesc(name, action, amount));

	if (currentPlayer)
	{
		const pcmn::Player::Ptr nextPlayer = currentPlayer->GetNext();
		if (nextPlayer)
		{
			if (action == pcmn::Action::SmallBlind)
				PlayerAction(nextPlayer->Name(), pcmn::Action::BigBlind, amount * 2);

			if (pcmn::Player::ThisPlayer().Name() == nextPlayer->Name())
				OnBotAction(); // our turn to play
		}
	}

	if (action == pcmn::Action::SecondsLeft && pcmn::Player::ThisPlayer().Name() == name)
		OnBotAction(); // our turn to play
}

void Table::FlopCards(const pcmn::Card::List& cards)
{
	Phase::Value phase = Phase::Preflop;
	switch (cards.size())
	{
	case 3: phase = Phase::Flop; break;
	case 4: phase = Phase::Turn; break;
	case 5: phase = Phase::River; break;
	default: assert(false);
	}

	m_FlopCards = cards;
	SetPhase(phase);
}

void Table::BotCards(const pcmn::Card& first, const pcmn::Card& second)
{
	m_PlayerCards[pcmn::Player::ThisPlayer().Name()] = boost::assign::list_of(first)(second);
}

void Table::PlayersInfo(const pcmn::Player::List& players)
{
	SendStatistic();
	ResetPhase();

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
			return player->Name() == name;
		}
	);

	if (it != m_Players.end())
		return *it;

	return pcmn::Player::Ptr();
}

void Table::OnBotAction()
{
	// make a decision and react
	SendStatistic();

	m_Connection->Receive(boost::bind(&Table::ReceiveFromServerCallback, this, _1), &net::Reply());
}

void Table::PlayerCards(const std::string& name, const pcmn::Card::List& cards)
{
	m_PlayerCards[name] = cards;
}

void Table::ResetPhase()
{
	m_Phase = Phase::Preflop;
	m_FlopCards.clear();
	m_Actions.clear();
	m_Actions.resize(4);
	m_PlayerCards.clear();
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

void Table::ParsePlayers(std::string& button)
{
	// parse player queue
	const Actions& actions = m_Actions[Phase::Preflop];

	assert(actions.at(0).m_Value == pcmn::Action::SmallBlind);

	if (!m_Players.empty())
	{
		const pcmn::Player::Ptr smallBlind = GetPlayer(actions.at(0).m_Name);
		if (smallBlind)
		{
			const pcmn::Player::Ptr onButton = smallBlind->GetPrevious();
			if (onButton)
			{
				// try to match all players
				std::set<std::string> actionPlayers;
				for (const ActionDesc& action : actions)
					actionPlayers.insert(action.m_Name);

				std::set<std::string> parsedPlayers;
				for (const pcmn::Player::Ptr& player : m_Players)
					parsedPlayers.insert(player->Name());

				button = onButton->Name(); // found previous player

				if (actionPlayers == parsedPlayers)
					return; // list is actual
			}
		}
		m_Players.clear(); // invalid player list
		button = actions.back().m_Name; // set last player as on button, because we may don't know really player
	}

	std::string smallBlind;
	bool bigBlindIsNext = false;
	for (unsigned i = 0; i < actions.size(); ++i)
	{
		const ActionDesc& action = actions.at(i);

		unsigned stack;
		if (m_Stacks.count(action.m_Name))
			stack = m_Stacks[action.m_Name];
		else
			stack = 1500;

		if (bigBlindIsNext)
		{
			assert(m_Players.size() > 1);

			m_Players[1] = boost::make_shared<pcmn::Player>(action.m_Name, stack);

			// make links
			m_Players.front()->SetPrevious(m_Players.back());
			m_Players.back()->SetNext(m_Players.front());

			for (unsigned p = 0; p < m_Players.size() - 1; ++p)
			{
				m_Players[p]->SetNext(m_Players[p + 1]);
				m_Players[p + 1]->SetPrevious(m_Players[p]);
			}

			break;
		}

		if (smallBlind.empty())
		{
			// next player - on big blind, reserve space
			m_Players.push_back(boost::make_shared<pcmn::Player>(action.m_Name, stack));

			if (actions[i + 1].m_Value != pcmn::Action::BigBlind)
				m_Players.push_back(pcmn::Player::Ptr());

			smallBlind = action.m_Name;
		}
		else
		if (smallBlind == action.m_Name)
		{
			// next action will be from big blind
			// previous action was from button
			bigBlindIsNext = true;
			button = actions[i - 1].m_Name;
		}
		else
		{
			m_Players.push_back(boost::make_shared<pcmn::Player>(action.m_Name, stack));
		}
	}	
}

void Table::SendStatistic()
{
	if (m_Actions[Phase::Preflop].empty())
		return;

	std::string onButton;
	ParsePlayers(onButton);

	assert(!onButton.empty());

	if (m_Actions[Phase::Preflop].at(1).m_Value != pcmn::Action::BigBlind)
	{
		const pcmn::Player::Ptr smallBlind = GetPlayer(m_Actions[Phase::Preflop].at(0).m_Name);
		assert(smallBlind);
		const pcmn::Player::Ptr bigBlind = smallBlind->GetNext();
		assert(bigBlind);
		m_Actions[Phase::Preflop].insert(m_Actions[Phase::Preflop].begin() + 1, ActionDesc(bigBlind->Name(), pcmn::Action::BigBlind, m_Actions[Phase::Preflop].at(0).m_Amount * 2));
	}

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
		added.set_stack(m_Stacks[player->Name()]);

		const pcmn::Card::List& cards = m_PlayerCards[player->Name()];
		for (const pcmn::Card& card : cards)
			added.mutable_cards()->Add(card.ToEvalFormat());

		if (player->Name() == onButton)
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

	const pcmn::Action::Value action = static_cast<pcmn::Action::Value>(reply.action());

	switch (action)
	{
	case pcmn::Action::Fold:
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

	boost::random::uniform_int_distribution<> wait(100, 4000);

	boost::this_thread::interruptible_wait(wait(g_Random));

	RECT rect;
	CHECK(GetWindowRect(m_Window, &rect));

	const float resultX = static_cast<float>(rect.right - rect.left) / x;
	const float resultY = static_cast<float>(rect.bottom - rect.top) / y;

	LOG_TRACE("Pressing to: x='%s', y='%s'") % resultX % resultY;

	const LPARAM param = MAKELPARAM(resultX, resultY);
	CHECK(PostMessage(m_Window, WM_LBUTTONDOWN, (WPARAM)MK_LBUTTON, param));

	boost::random::uniform_int_distribution<> up(50, 200);

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


void Table::Ante(std::size_t value)
{
	m_Ante = value;
}

void Table::BetAnte()
{
	for (const pcmn::Player::Ptr& player : m_Players)
		m_Stacks[player->Name()] -= m_Ante;
}

void Table::RemovePlayer(const std::string& name)
{
	const pcmn::Player::List::iterator it = std::find_if
	(
		m_Players.begin(),
		m_Players.end(),
		[&](const pcmn::Player::Ptr& player)
		{
			return player->Name() == name;
		}
	);

	CHECK(it != m_Players.end(), "Failed to find player by name", name);
	(*it)->DeleteLinks();
	m_Players.erase(it);
}


} // namespace ps
} // namespace clnt


