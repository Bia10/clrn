#include "Messages.h"
#include "Modules.h"
#include "PokerStars.h"
#include "Actions.h"
#include "ITable.h"
#include "Cards.h"
#include "Exception.h"

#include <sstream>
#include <iomanip>

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
	const int amount = _byteswap_ulong(*reinterpret_cast<const int*>(&data[0x2d])) / 100;	

	LOG_TRACE("Player: '%s', action: '%p', amount: '%s'") % name % Action::ToString(actionValue) % amount;

	CHECK(actionValue != Action::Unknown, static_cast<int>(action));
	table.PlayerAction(name, actionValue, amount);
}


std::size_t FlopCards::GetId() const 
{
	return 0x1001;
}

void FlopCards::Process(const dasm::WindowMessage& message, ITable& table) const 
{
	SCOPED_LOG(m_Log);

	std::vector<Card> cards;
	FindFlopCards(message.m_Block, cards);

	if (cards.size() < 3)
	{
		std::vector<char> data;
		std::ostringstream oss;
		BytesToString(oss, message.m_Block.m_Data + message.m_Block.m_Offset, message.m_Block.m_Offset + message.m_Block.m_Size);
		LOG_ERROR("Failed to find flop cards, raw data: '%s'") % oss.str();
		CHECK(false, "Failed to find flop cards");
	}

	if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Trace))
	{
		std::ostringstream oss;
		for (const Card& card : cards)
			oss << "('" << Card::ToString(card.m_Value) << "' of '" << Suit::ToString(card.m_Suit) << "')";

		LOG_TRACE("Cards received: %s") % oss.str();
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
	SCOPED_LOG(m_Log);

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
	SCOPED_LOG(m_Log);

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

		LOG_TRACE("Player: '%s', stack: '%s', country: '%s'") % name % stack % country;

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