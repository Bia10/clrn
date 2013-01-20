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

Table::Table(ILog& logger) 
	: m_Log(logger)
	, m_Phase(Phase::Preflop)
	, m_SmallBlindAmount(0)
	, m_Pot(0)
	, m_Evaluator(new Calculator())
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
			LOG_TRACE("Message [0x%x] ignored, data: [%s]") % messageId % oss.str();
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

void Table::PlayerAction(const std::string& name, Action::Value action, std::size_t amount)
{
	if (m_Players.empty())
		return; 

	Player& current = GetPlayer(name);
	Player& next = GetNextPlayer(name);
	switch (action)
	{
		case Action::Fold: 
			current.SetStyle(static_cast<std::size_t>(m_Phase), Player::Style::Passive);
			current.Result(Player::Result::LooseWithFold);
			current.State(Player::State::Fold);
			break;
		case Action::Check: 
			current.SetStyle(static_cast<std::size_t>(m_Phase), Player::Style::Passive);
			current.State(Player::State::InPot);
			break;
		case Action::Call: 
			current.SetStyle(static_cast<std::size_t>(m_Phase), Player::Style::Normal);
			current.Stack(current.Stack() - amount);
			current.Bet(current.Bet() + amount);
			current.State(Player::State::InPot);
			m_Pot += amount;
			if (!current.Stack())
				current.State(Player::State::AllIn);
			break;
		case Action::Bet: 
			current.SetStyle(static_cast<std::size_t>(m_Phase), Player::Style::Agressive);
			current.Stack(current.Stack() - amount);
			current.Bet(current.Bet() + amount);
			current.State(Player::State::InPot);
			m_Pot += amount;
			if (!current.Stack())
				current.State(Player::State::AllIn);
			break;
		case Action::Raise:
			current.SetStyle(static_cast<std::size_t>(m_Phase), Player::Style::Agressive);
			current.Stack(current.Stack() - amount);
			current.Bet(current.Bet() + amount);
			current.State(Player::State::InPot);
			m_Pot += amount;
			if (!current.Stack())
				current.State(Player::State::AllIn);
			break;
		case Action::SmallBlind: 
			m_SmallBlindAmount = amount;
			current.Bet(m_SmallBlindAmount);
			next.Bet(m_SmallBlindAmount * 2);
			current.State(Player::State::InPot);
			next.State(Player::State::InPot);
			break;
		case Action::Ante: 
			m_Pot += amount;
			current.Stack(current.Stack() - amount);
			break;
		case Action::MoneyReturn: 
			current.Stack(current.Stack() + amount);
			current.Bet(current.Bet() - amount);
			m_Pot -= amount;
			break;
		case Action::SecondsLeft: 
			if (name == Player::ThisPlayer().Name())
				OnBotAction(); // our turn to play
			break;
		case Action::ShowCards: 
			break;
		case Action::Win: 
			break;
		case Action::Loose: 
			break;
		case Action::Rank: 
			break;
		case Action::WinCards: 
			break;
		default: assert(false);
	}

	if (action != Action::SmallBlind && IsPhaseCompleted(current))
	{
		LOG_TRACE("Switching state from '%d' to '%d'") % m_Phase % (static_cast<int>(m_Phase) + 1);

		if (m_Phase == Phase::River)
			ProcessWinners();

		return;
	}

	if (Player::ThisPlayer().Name() == next.Name())
		OnBotAction(); // our turn to play
}

void Table::FlopCards(const Card::List& cards)
{
	for (Player& player : m_Players)
		player.State(Player::State::Waiting);

	switch (cards.size())
	{
	case 3: m_Phase = Phase::Flop; break;
	case 4: m_Phase = Phase::Turn; break;
	case 5: m_Phase = Phase::River; break;
	default: assert(false);
	}

	m_FlopCards = cards;
}

void Table::BotCards(const Card& first, const Card& second)
{
	m_PlayerCards.clear();
	m_PlayerCards.push_back(first);
	m_PlayerCards.push_back(second);
}

void Table::PlayersInfo(const Player::List& players)
{
	ResetPhase();
	m_Players = players;
}

Player& Table::GetNextPlayer(const std::string& name)
{
	const Player::List::iterator it = std::find_if
	(
		m_Players.begin(),
		m_Players.end(),
		[&](const Player& player)
		{
			return player.Name() == name;
		}
	);

	CHECK(it != m_Players.end(), "Failed to find player by name", name);
	if (it + 1 == m_Players.end())
		return m_Players.front();

	return *(it + 1);
}

Player& Table::GetPlayer(const std::string& name)
{
	const Player::List::iterator it = std::find_if
	(
		m_Players.begin(),
		m_Players.end(),
		[&](const Player& player)
		{
			return player.Name() == name;
		}
	);

	CHECK(it != m_Players.end(), "Failed to find player by name", name);
	return *it;
}

Player& Table::GetNextActivePlayer(const std::string& name)
{
	// find next active player
	for (Player* next = &GetNextPlayer(name); next->Name() != name; next = &GetNextPlayer(next->Name()))
	{
		if (next->State() != Player::State::Fold)
			return *next;
	}

	return GetPlayer(name); // no active players
}


void Table::OnBotAction()
{
	// make a decision and react
}

bool Table::IsPhaseCompleted(Player& current)
{
	// check all bets here
	// if all bets are off - going to next phase
	// if some players are all in - create additional pots and go to next phase

	// find any waiting player
	const Player::List::const_iterator it = std::find_if(m_Players.begin(), m_Players.end(), [](const Player& p){ return p.State() == Player::State::Waiting; });
	if (it != m_Players.end())
		return false;

	// calculate win sizes
	std::size_t maxBet = 0;
	for (Player& player : m_Players)
	{
		if (player.Bet() > maxBet)
			maxBet = player.Bet();

		switch (player.State())
		{
		case Player::State::AllIn:
			{
				player.WinSize(m_Pot);
				for (const Player& current : m_Players)
				{
					if (current.State() == Player::State::Fold)
						continue;

					if (current.Bet() < player.Bet())
					{
						const std::size_t diff = player.Bet() - current.Bet();
						player.WinSize(player.WinSize() - diff);
					}
				}

			}
			break;
		case Player::State::Fold:
			break;
		case Player::State::InPot:
			player.WinSize(m_Pot);
			break;
		}
	}

	return true;
}

void Table::PlayerCards(const std::string& name, const std::string& cards)
{
	if (m_Players.empty())
		return;

	assert(cards.size() == 4);

	Player& player = GetPlayer(name);
	Card::List cardsList;

	Card tmp(Card::FromString(cards[0]), static_cast<Suit::Value>(cards[1]));
	cardsList.push_back(tmp);

	tmp.m_Value = Card::FromString(cards[2]);
	tmp.m_Suit = static_cast<Suit::Value>(cards[3]);
	cardsList.push_back(tmp);

	player.Cards(cardsList);
}

void Table::ResetPhase()
{
	m_Phase = Phase::Preflop;
	m_Players.clear();
	m_PlayerCards.clear();
	m_Pot = 0;
	m_FlopCards.clear();
	m_SmallBlindAmount = 0;
}

void Table::ProcessWinners()
{
	assert(m_FlopCards.size() == 5);

	struct Info
	{
		std::string m_Name;
		short m_Value;
		std::size_t m_Pot;
	};

	std::vector<Info> players;
	std::size_t maxPot = 0;
	for (const Player& player : m_Players)
	{
		const Card::List& cards = player.Cards();
		if (!cards.empty() && player.State() != Player::State::Fold)
		{
			players.push_back(Info());
			Info& info = players.back();

			Card::List playerCards = m_FlopCards;
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
	for (const Info& player : players)
	{
		if (winners)
			GetPlayer(player.m_Name).Result(Player::Result::WinByCards);
		else
			GetPlayer(player.m_Name).Result(Player::Result::LooseWithCards);

		if (player.m_Pot != maxPot)
		{
			addPot = player.m_Pot;
			LOG_TRACE("Player '%s' wins additional pot: '%s' with hand: '%s'") % player.m_Name % player.m_Pot % Combination::ToString(Combination::FromEval(player.m_Value));
		}
		else
		{
			LOG_TRACE("Player '%s' wins main pot: '%s' with hand: '%s'") % player.m_Name % (player.m_Pot - addPot) % Combination::ToString(Combination::FromEval(player.m_Value));
			winners = false;
		}
	}
}

void Table::SendStatistic()
{

}


} // namespace ps
} // namespace clnt
