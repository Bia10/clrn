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
	, m_Evaluator(new Evaluator())
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
	m_Actions[m_Phase].push_back(ActionDesc(name, action, amount));

	LOG_TRACE("Player: '%s', action: '%s', amount: '%s'") % name % Action::ToString(action) % amount;

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
		case Action::Bet: 
		case Action::Raise:
			current.SetStyle(static_cast<std::size_t>(m_Phase), Player::Style::Agressive);
		case Action::Call: 
			{
				current.SetStyle(static_cast<std::size_t>(m_Phase), Player::Style::Normal);

				const std::size_t difference = amount - current.Bet();
				current.Stack(current.Stack() - difference);
				current.Bet(amount);
				current.State(Player::State::InPot);
				m_Pot += difference;
				if (!current.Stack())
					current.State(Player::State::AllIn);
			}
			break;
		case Action::SmallBlind: 
			assert(!m_SmallBlindAmount);
			m_OnButton = current.Name();
			m_SmallBlindAmount = amount;
			current.Bet(m_SmallBlindAmount);
			m_Pot += m_SmallBlindAmount;
			next.Bet(m_SmallBlindAmount * 2);
			m_Pot += m_SmallBlindAmount * 2;
			current.State(Player::State::InPot);
			next.State(Player::State::InPot);
			LOG_TRACE("Player: '%s', action: '%s', amount: '%s'") % next.Name() % Action::ToString(Action::BigBlind) % (m_SmallBlindAmount * 2);
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

	std::size_t playersInPot = 0;
	if (action != Action::SmallBlind && IsPhaseCompleted(current, playersInPot))
	{
		LOG_TRACE("Switching state from '%d' to '%d'") % m_Phase % (static_cast<int>(m_Phase) + 1);

		if (m_Phase == Phase::River || playersInPot == 1)
			ProcessWinners(playersInPot);

		return;
	}

	if (Player::ThisPlayer().Name() == next.Name())
		OnBotAction(); // our turn to play
}

void Table::FlopCards(const Card::List& cards)
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

void Table::BotCards(const Card& first, const Card& second)
{
	m_BotCards.clear();
	m_BotCards.push_back(first);
	m_BotCards.push_back(second);
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

bool Table::IsPhaseCompleted(Player& current, std::size_t& playersInPot)
{
	// check all bets here
	// if all bets are off - going to next phase
	// if some players are all in - create additional pots and go to next phase

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
		case Player::State::Waiting:
			return false;
			break;
		}
	}

	// find players that didn't call bets
	playersInPot = 0;
	bool finished = true;
	for (const Player& player : m_Players)
	{
		if (player.State() == Player::State::InPot)
		{
			++playersInPot;
			if (player.Bet() < maxBet)
				finished = false;
		}
	}

	return finished;
}

void Table::PlayerCards(const std::string& name, const Card::List& cards)
{
	if (m_Players.empty())
		return;

	assert(cards.size() == 2);
	Player& player = GetPlayer(name);
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
		Player* playerPtr = 0;
		for (Player& player : m_Players)
		{
			if (player.State() == Player::State::InPot)
			{
				assert(!playerPtr);
				playerPtr = &player;
			}
		}

		const Player::Style::Value style = playerPtr->GetStyle(m_Phase);
		if (style == Player::Style::Agressive)
		{
			playerPtr->Result(Player::Result::WinByRaise);
			LOG_TRACE("Player '%s' wins main pot: '%s' by successfull raise.") % playerPtr->Name() % m_Pot;
		}
		else
		{
			playerPtr->Result(Player::Result::WinByLuck);
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
	short winValue = 0;
	for (const Info& player : players)
	{
		if (winners || player.m_Value == winValue)
		{
			GetPlayer(player.m_Name).Result(Player::Result::WinByCards);
			if (player.m_Pot != maxPot)
			{
				addPot = player.m_Pot;
				LOG_TRACE("Player '%s' wins additional pot: '%s' with hand: '%s'") % player.m_Name % player.m_Pot % Combination::ToString(Combination::FromEval(player.m_Value));
			}
			else
			{
				LOG_TRACE("Player '%s' wins main pot: '%s' with hand: '%s'") % player.m_Name % (player.m_Pot - addPot) % Combination::ToString(Combination::FromEval(player.m_Value));
				winners = false;
				winValue = player.m_Value;
			}
		}
		else
		{
			GetPlayer(player.m_Name).Result(Player::Result::LooseWithCards);
			LOG_TRACE("Player '%s' loose with hand: '%s'") % player.m_Name % Combination::ToString(Combination::FromEval(player.m_Value));
		}
	}
}

void Table::SetPhase(const Phase::Value phase)
{
	for (Player& p : m_Players)
	{
		p.Bet(0);
		if (p.State() != Player::State::Fold)
			p.State(Player::State::Waiting);
	}

	m_Phase = phase;
}

void Table::SendStatistic()
{

}


} // namespace ps
} // namespace clnt
