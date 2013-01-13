#include "Messages.h"
#include "Modules.h"
#include "PokerStars.h"
#include "Actions.h"
#include "ITable.h"
#include "Cards.h"

#include <sstream>

namespace clnt
{
namespace ps
{
namespace msg
{

const std::size_t CURRENT_MODULE_ID = Modules::Messages;

Action::Value ConvertAction(const char action)
{
	switch (action)
	{
	case 'F': return Action::Fold;
	case 'C': return Action::Call;
	case 'c': return Action::Check;
	case 'B': return Action::Bet;
	case 'E': return Action::Raise;
	case 'S': return Action::Show;
	case 's': return Action::Loose;
	case 'P': return Action::BigBlind;
	case 'p': return Action::SmallBlind;
	default: return Action::Unknown;
	}

	// to find M, Q, q
}

bool IsCard(const char* data)
{
	if (data[0] != 'd' && data[0] != 's' && data[0] != 'h' && data[0] != 'c')
		return false;

	if (data[1] < 0x02 || data[1] > 0x0e)
		return false;
	return true;
}

bool IsCardsEnd(const char* data)
{
	return data[0] == 0x75 && data[1] == 0x00;
}

void FindFlopCards(const dasm::DataBlock& data, std::vector<Card>& result)
{
	int cardsFound = 0;
	for (unsigned int i = data.m_Offset ; i < data.m_Size + data.m_Offset; ++i)
	{
		const char* current = data.m_Data + i;
		if (IsCard(current))
		{
			++cardsFound;
			++i;

			Card currentCard;
			currentCard.m_Suit = static_cast<Suit::Value>(current[0]);
			currentCard.m_Value = static_cast<Card::Value>(current[1]);

			result.push_back(currentCard);
		}
		else
		if (cardsFound)
		{
			if (cardsFound >= 3 && IsCardsEnd(current))
				return;

			cardsFound = 0;
			result.clear();
		}
	}
}

std::size_t PlayerAction::GetId() const 
{
	return 0x10000;
}

void PlayerAction::Process(const dasm::WindowMessage& message, ITable& table) const 
{
	SCOPED_LOG(m_Log);

	const char* data = message.m_Block.m_Data + message.m_Block.m_Offset;

	const char* name = &data[0x31];
	const char action = data[0x2c];
	const Action::Value actionValue = ConvertAction(action);
	const int amount = _byteswap_ulong(*reinterpret_cast<const int*>(&data[0x2d]));	

	LOG_TRACE("Player name: [%s], action: [%p], amount: [%s]") % name % Action::ToString(actionValue) % amount;

	if (actionValue == Action::Unknown)
		LOG_WARNING("Unknown action: [%p][%s]") % action % action;


	table.PlayerAction(name, actionValue, amount);
}


std::size_t FlopCards::GetId() const 
{
	return 0x1001;
}

void FlopCards::Process(const dasm::WindowMessage& message, ITable& table) const 
{
	std::vector<Card> cards;
	FindFlopCards(message.m_Block, cards);

	assert(cards.size() >= 3);

	if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Trace))
	{
		std::ostringstream oss;
		for (const Card& card : cards)
			oss << "[" << Card::ToString(card.m_Value) << " of " << Suit::ToString(card.m_Suit) << "]";

		LOG_TRACE("Cards received: [%s]") % oss.str();
	}

	table.FlopCards(cards);
}


std::size_t TurnAndRiverCards::GetId() const 
{
	return 0x1005;
}

std::size_t PlayerCards::GetId() const 
{
	return 0x1800;
}

void PlayerCards::Process(const dasm::WindowMessage& message, ITable& table) const 
{
	const char* data = message.m_Block.m_Data + message.m_Block.m_Offset;

	Card first;
	first.m_Suit = static_cast<Suit::Value>(data[0x15]);
	first.m_Value = static_cast<Card::Value>(data[0x16]);

	Card second;
	second.m_Suit = static_cast<Suit::Value>(data[0x18]);
	second.m_Value = static_cast<Card::Value>(data[0x19]);

	table.PlayerCards(first, second);
}


std::size_t PlayersInfo::GetId() const 
{
	return 0x20000;
}

void PlayersInfo::Process(const dasm::WindowMessage& message, ITable& table) const 
{
	dasm::MessageRecord record(message);
	record.SetParsedBytes(0x20);

	std::ostringstream oss;

	Player::List players;
	for (;;)
	{
		std::string name;
		record.Extract(name);

		if (name.empty())
			break;

		DWORD stack;
		record.Extract(stack);
		stack = _byteswap_ulong(stack) / 100;

		std::string country;
		record.Extract(country);

		DWORD temp;
		record.Extract(temp);
		record.Extract(temp);

		LOG_TRACE("Player: [%s], stack: [%s], country: [%s]") % name % stack, country;

		Player player;
		player.Name(name);
		player.Country(country);
		player.Stack(stack);

		players.push_back(player);
	}

	table.PlayersInfo(players);
}

} // namespace msg
} // namespace ps
} // namespace clnt