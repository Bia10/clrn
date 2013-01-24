#include "PokerStarsTable.h"
#include "Messages.h"
#include "PokerStars.h"
#include "Modules.h"
#include "packet.pb.h"

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

Table::Table(ILog& logger, const net::IConnection::Ptr& connection) 
	: m_Log(logger)
	, m_Phase(Phase::Preflop)
	, m_SmallBlindAmount(0)
	, m_Pot(0)
	, m_Evaluator(new pcmn::Evaluator())
	, m_Actions(4)
	, m_Connection(connection)
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
		const std::auto_ptr<IMessage> handler(s_Factory.Create(messageId));
	
		if (!handler.get())
		{
			std::vector<char> data;
			std::ostringstream oss;
			BytesToString(oss, message.m_Block.m_Data + message.m_Block.m_Offset, message.m_Block.m_Offset + message.m_Block.m_Size);
			LOG_DEBUG("Message [0x%x] ignored, data: [%s]") % messageId % oss.str();
			return;
		}
		
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
	const bool firstMove = m_Actions[m_Phase].empty();
	m_Actions[m_Phase].push_back(ActionDesc(name, action, amount));

	LOG_TRACE("Player: '%s', action: '%s', amount: '%s'") % name % pcmn::Action::ToString(action) % amount;

	if (m_Players.empty())
		return; 

	if (firstMove && action != pcmn::Action::SmallBlind)
		m_OnButton = GetPreviousPlayer(GetPreviousPlayer(name).Name()).Name();

	pcmn::Player& current = GetPlayer(name);
	pcmn::Player& next = GetNextPlayer(name);
	switch (action)
	{
		case pcmn::Action::Fold: 
			current.SetStyle(static_cast<std::size_t>(m_Phase), pcmn::Player::Style::Passive);
			current.Result(pcmn::Player::Result::LooseWithFold);
			current.State(pcmn::Player::State::Fold);
			break;
		case pcmn::Action::Check: 
			current.SetStyle(static_cast<std::size_t>(m_Phase), pcmn::Player::Style::Passive);
			current.State(pcmn::Player::State::InPot);
			break;
		case pcmn::Action::Bet: 
		case pcmn::Action::Raise:
			{
				current.SetStyle(static_cast<std::size_t>(m_Phase), pcmn::Player::Style::Agressive);

				const std::size_t difference = amount - current.Bet();
				current.Stack(current.Stack() - difference);
				current.Bet(amount);
				current.State(pcmn::Player::State::InPot);
				m_Pot += difference;
				if (!current.Stack())
					current.State(pcmn::Player::State::AllIn);
			}
			break;
		case pcmn::Action::Call: 
			{
				current.SetStyle(static_cast<std::size_t>(m_Phase), pcmn::Player::Style::Normal);

				const std::size_t difference = amount;
				current.Stack(current.Stack() - difference);
				current.Bet(current.Bet() + difference);
				current.State(pcmn::Player::State::InPot);
				m_Pot += difference;
				if (!current.Stack())
					current.State(pcmn::Player::State::AllIn);
			}
			break;
		case pcmn::Action::SmallBlind: 
			assert(!m_SmallBlindAmount);
			m_OnButton = current.Name();
			m_SmallBlindAmount = amount;
			current.Bet(m_SmallBlindAmount);
			m_Pot += m_SmallBlindAmount;
			next.Bet(m_SmallBlindAmount * 2);
			m_Pot += m_SmallBlindAmount * 2;
			current.State(pcmn::Player::State::Waiting);
			next.State(pcmn::Player::State::Waiting);
			LOG_TRACE("Player: '%s', action: '%s', amount: '%s'") % next.Name() % pcmn::Action::ToString(pcmn::Action::BigBlind) % (m_SmallBlindAmount * 2);
			break;
		case pcmn::Action::Ante: 
			m_Pot += amount;
			current.Stack(current.Stack() - amount);
			break;
		case pcmn::Action::MoneyReturn: 
			current.Stack(current.Stack() + amount);
			current.Bet(current.Bet() - amount);
			m_Pot -= amount;
			return;
		case pcmn::Action::SecondsLeft: 
			if (name == pcmn::Player::ThisPlayer().Name())
				OnBotAction(); // our turn to play
			break;
		case pcmn::Action::ShowCards: 
		case pcmn::Action::Win: 
		case pcmn::Action::Loose: 
		case pcmn::Action::Rank: 
			if (current.Name() == pcmn::Player::ThisPlayer().Name())
				CloseWindow();
		case pcmn::Action::WinCards: 
			return;
		default: assert(false);
	}

	std::size_t playersInPot = 0;
	if (action != pcmn::Action::SmallBlind && IsPhaseCompleted(current, playersInPot))
	{
		LOG_TRACE("Switching state from '%d' to '%d'") % m_Phase % (static_cast<int>(m_Phase) + 1);

		if (m_Phase == Phase::River || playersInPot == 1)
			ProcessWinners(playersInPot);

		return;
	}

	if (pcmn::Player::ThisPlayer().Name() == next.Name())
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
	m_BotCards.clear();
	m_BotCards.push_back(first);
	m_BotCards.push_back(second);
}

void Table::PlayersInfo(const pcmn::Player::List& players)
{
	SendStatistic();
	ResetPhase();
	m_Players = players;
}

pcmn::Player& Table::GetNextPlayer(const std::string& name)
{
	const pcmn::Player::List::iterator it = std::find_if
	(
		m_Players.begin(),
		m_Players.end(),
		[&](const pcmn::Player& player)
		{
			return player.Name() == name;
		}
	);

	CHECK(it != m_Players.end(), "Failed to find player by name", name);
	if (it + 1 == m_Players.end())
		return m_Players.front();

	return *(it + 1);
}

pcmn::Player& Table::GetPlayer(const std::string& name)
{
	const pcmn::Player::List::iterator it = std::find_if
	(
		m_Players.begin(),
		m_Players.end(),
		[&](const pcmn::Player& player)
		{
			return player.Name() == name;
		}
	);

	CHECK(it != m_Players.end(), "Failed to find player by name", name);
	return *it;
}

pcmn::Player& Table::GetNextActivePlayer(const std::string& name)
{
	// find next active player
	for (pcmn::Player* next = &GetNextPlayer(name); next->Name() != name; next = &GetNextPlayer(next->Name()))
	{
		if (next->State() != pcmn::Player::State::Fold)
			return *next;
	}

	return GetPlayer(name); // no active players
}


pcmn::Player& Table::GetPreviousPlayer(const std::string& name)
{
	for (std::size_t i = 0; i < m_Players.size(); ++i)
	{
		if (m_Players[i].Name() == name)
		{
			if (i)
				return m_Players[i - 1];
			else
				return m_Players[m_Players.size() - 1];
		}
	}

	assert(false);
	return GetPlayer(name); 
}


void Table::OnBotAction()
{
	// make a decision and react
}

bool Table::IsPhaseCompleted(pcmn::Player& current, std::size_t& playersInPot)
{
	// check all bets here
	// if all bets are off - going to next phase
	// if some players are all in - create additional pots and go to next phase

	std::size_t maxBet = 0;
	for (pcmn::Player& player : m_Players)
	{
		if (player.Bet() > maxBet)
			maxBet = player.Bet();

		switch (player.State())
		{
		case pcmn::Player::State::AllIn:
			{
				player.WinSize(m_Pot);
				for (const pcmn::Player& current : m_Players)
				{
					if (current.State() == pcmn::Player::State::Fold)
						continue;

					if (current.Bet() < player.Bet())
					{
						const std::size_t diff = player.Bet() - current.Bet();
						player.WinSize(player.WinSize() - diff);
					}
				}

			}
			break;
		case pcmn::Player::State::Fold:
			break;
		case pcmn::Player::State::InPot:
			player.WinSize(m_Pot);
			break;
		case pcmn::Player::State::Waiting:
			return false;
			break;
		}
	}

	// find players that didn't call bets
	playersInPot = 0;
	bool finished = true;
	for (const pcmn::Player& player : m_Players)
	{
		if (player.State() == pcmn::Player::State::InPot || player.State() == pcmn::Player::State::AllIn)
		{
			++playersInPot;
			if (player.State() != pcmn::Player::State::AllIn && player.Bet() < maxBet)
				finished = false;
		}
	}

	return finished;
}

void Table::PlayerCards(const std::string& name, const pcmn::Card::List& cards)
{
	if (m_Players.empty())
		return;

	assert(cards.size() == 2);
	pcmn::Player& player = GetPlayer(name);
	player.Cards(cards);
}

void Table::ResetPhase()
{
	m_Phase = Phase::Preflop;
	m_Players.clear();
	m_BotCards.clear();
	m_Pot = 0;
	m_FlopCards.clear();
	m_SmallBlindAmount = 0;
	m_Actions.clear();
	m_Actions.resize(4);
	m_OnButton.clear();
}

void Table::ProcessWinners(const std::size_t playersInPot)
{
	if (playersInPot == 1)
	{
		// all players folds
		pcmn::Player* playerPtr = 0;
		for (pcmn::Player& player : m_Players)
		{
			if (player.State() == pcmn::Player::State::InPot || player.State() == pcmn::Player::State::AllIn)
			{
				assert(!playerPtr);
				playerPtr = &player;
			}
		}

		const pcmn::Player::Style::Value style = playerPtr->GetStyle(m_Phase);
		if (style == pcmn::Player::Style::Agressive)
		{
			playerPtr->Result(pcmn::Player::Result::WinByRaise);
			LOG_TRACE("Player '%s' wins main pot: '%s' by successfull raise.") % playerPtr->Name() % m_Pot;
		}
		else
		{
			playerPtr->Result(pcmn::Player::Result::WinByLuck);
			LOG_TRACE("Player '%s' wins main pot: '%s' because all players folds.") % playerPtr->Name() % m_Pot;
		}

		return;
	}

	assert(m_FlopCards.size() == 5);

	struct Info
	{
		std::string m_Name;
		short m_Value;
		std::size_t m_Pot;
	};

	std::vector<Info> players;
	std::size_t maxPot = 0;
	for (const pcmn::Player& player : m_Players)
	{
		const pcmn::Card::List& cards = player.Cards();
		if (!cards.empty() && player.State() != pcmn::Player::State::Fold)
		{
			players.push_back(Info());
			Info& info = players.back();

			pcmn::Card::List playerCards = m_FlopCards;
			std::copy(cards.begin(), cards.end(), std::back_inserter(playerCards));

			assert(playerCards.size() == 7);
			
			info.m_Name = player.Name();
			info.m_Pot = player.WinSize();
			info.m_Value = m_Evaluator->GetRank(playerCards);

			if (info.m_Pot > maxPot)
				maxPot = info.m_Pot;
		}
	}

	std::sort(players.begin(), players.end(), [](const Info& lhs, const Info& rhs) { return lhs.m_Value > rhs.m_Value; });

	std::size_t addPot = 0;
	bool winners = true;
	short winValue = 0;
	for (const Info& player : players)
	{
		if (winners || player.m_Value == winValue)
		{
			GetPlayer(player.m_Name).Result(pcmn::Player::Result::WinByCards);
			if (player.m_Pot != maxPot)
			{
				addPot = player.m_Pot;
				LOG_TRACE("Player '%s' wins additional pot: '%s' with hand: '%s'") % player.m_Name % player.m_Pot % pcmn::Combination::ToString(pcmn::Combination::FromEval(player.m_Value));
			}
			else
			{
				LOG_TRACE("Player '%s' wins main pot: '%s' with hand: '%s'") % player.m_Name % (player.m_Pot - addPot) % pcmn::Combination::ToString(pcmn::Combination::FromEval(player.m_Value));
				winners = false;
				winValue = player.m_Value;
			}
		}
		else
		{
			GetPlayer(player.m_Name).Result(pcmn::Player::Result::LooseWithCards);
			LOG_TRACE("Player '%s' loose with hand: '%s'") % player.m_Name % pcmn::Combination::ToString(pcmn::Combination::FromEval(player.m_Value));
		}
	}
}

void Table::SetPhase(const Phase::Value phase)
{
	for (pcmn::Player& p : m_Players)
	{
		p.Bet(0);
		if (p.State() != pcmn::Player::State::Fold && p.State() != pcmn::Player::State::AllIn)
			p.State(pcmn::Player::State::Waiting);
	}

	m_Phase = phase;
}

void Table::SendStatistic()
{
	if (m_Actions[Phase::Preflop].empty() || m_OnButton.empty())
		return;

	net::Packet packet;

	net::Packet::Table& table = *packet.mutable_info();

	std::size_t counter = 0;
	std::map<std::string, std::size_t> players;
	for (const pcmn::Player& player : m_Players)
	{
		net::Packet::Player& added = *table.add_players();
		added.set_bet(player.Bet());
		added.set_name(player.Name());
		added.set_stack(player.Stack());

		const pcmn::Card::List& cards = player.Cards();
		for (const pcmn::Card& card : cards)
			added.mutable_cards()->Add(card.ToEvalFormat());

		if (player.Name() == m_OnButton)
			table.set_button(counter);

		players[player.Name()] = counter;
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

void Table::CloseWindow()
{

}


} // namespace ps
} // namespace clnt