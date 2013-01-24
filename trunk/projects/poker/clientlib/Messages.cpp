#include "Messages.h"
#include "Modules.h"
#include "PokerStars.h"
#include "Actions.h"
#include "ITable.h"
#include "Cards.h"
#include "Exception.h"

#include <sstream>
#include <iomanip>

using namespace pcmn;

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
	case 'F' : return Action::Fold;
	case 'C' : return Action::Call;
	case 'c' : return Action::Check;
	case 'B' : return Action::Bet;
	case 'E' : return Action::Raise;
 	case 'S' : return Action::ShowCards;
 	case 's' : return Action::ShowCards;
	case 'P' : return Action::BigBlind;
	case 'p' : return Action::SmallBlind;
	case 'r' : return Action::MoneyReturn;
	case 'w' : return Action::Win;
	case 0x8 : return Action::SecondsLeft;
	case 0xB : return Action::Rank; // player out
	case 0x1 : return Action::WinCards; // win with combination of cards
	case 0x2 : return Action::WinCards; // win with combination of cards
	case 'M' : return Action::Loose; // cards not showed
	case 't' : return Action::SecondsLeft; // time left?
	default: return Action::Unknown;
	}

	// to find Q, q
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

			result.push_back(Card(static_cast<Card::Value>(current[1]), static_cast<Suit::Value>(current[0])));
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
	const char* end = message.m_Block.m_Data + message.m_Block.m_Offset + message.m_Block.m_Size;

	data = std::find(data + 0x0c, end, 0xff);
	data += 0x14;

	const char action = *data;

	if (!action)
		return; // empty message

	++data;

	int amount = _byteswap_ulong(*reinterpret_cast<const int*>(data));	

	data += sizeof(DWORD);

	const Action::Value actionValue = ConvertAction(action);

	const std::string name(data);
	if (actionValue == Action::ShowCards)
	{
		data += (name.size() + 1);
		const std::string cards(data);

		LOG_TRACE("Player: '%s', action: '%p', cards: '%s'") % name % Action::ToString(actionValue) % cards;

		if (!name.empty() && !cards.empty())
		{
			Card::List cardsList;

			Card tmp(Card::FromString(cards[0]), static_cast<Suit::Value>(cards[1]));
			cardsList.push_back(tmp);

			tmp.m_Value = Card::FromString(cards[2]);
			tmp.m_Suit = static_cast<Suit::Value>(cards[3]);
			cardsList.push_back(tmp);

			table.PlayerCards(name, cardsList);
		}
	}
	else
	if (actionValue == Action::WinCards)
	{
		data += (name.size() + 1);
		LOG_TRACE("Player: '%s', action: '%p', pot: '%s', cards: '%s'") % name % Action::ToString(actionValue) % amount % data;
	}
	else
	{
		LOG_TRACE("Player: '%s', action: '%p', amount: '%s'") % data % Action::ToString(actionValue) % amount;
	}

	CHECK(actionValue != Action::Unknown, static_cast<int>(action));
	if (!name.empty())
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
		return; // empty messsage;

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

	Card first(static_cast<Card::Value>(data[0x16]), static_cast<Suit::Value>(data[0x15]));
	Card second(static_cast<Card::Value>(data[0x19]), static_cast<Suit::Value>(data[0x18]));

	if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Trace))
	{
		std::ostringstream oss;
		oss << "('" << Card::ToString(first.m_Value) << "' of '" << Suit::ToString(first.m_Suit) << "')";
		oss << "('" << Card::ToString(second.m_Value) << "' of '" << Suit::ToString(second.m_Suit) << "')";

		LOG_TRACE("Player cards: %s") % oss.str();
	}

	table.BotCards(first, second);
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
		stack = _byteswap_ulong(stack);

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


std::size_t PlayerInfo::GetId() const 
{
	return 0x1006;
}

void PlayerInfo::Process(const dasm::WindowMessage& message, ITable& table) const 
{
	dasm::MessageRecord record(message);

	const char* data = message.m_Block.m_Data + message.m_Block.m_Offset;
	const char* end = message.m_Block.m_Data + message.m_Block.m_Offset + message.m_Block.m_Size;

	Player::List players;

	for (int i = 0 ; i < 5 && data < end; ++i)
		data = std::find(data + 1, end, 0xff);

	while (data < end)
	{
		data += 0x19;

		if (*data < 0x20)
			break;

		Player player;
		player.Name(data);

		players.push_back(player);
	
		for (int i = 0 ; i < 3 && data < end; ++i)
			data = std::find(data + 1, end, 0xff);
	}

	unsigned index = 0;
	data = message.m_Block.m_Data + message.m_Block.m_Offset;
	while (data < end) 
	{
		data = std::find(data + 1, end, 0x1c);
		if (*(data - 1) - char(0x00))
			continue;
		if (*(data - 2) - char(0x00))
			continue;
		if (*(data - 3) - char(0x00))
			continue;
		if (*(data - 4) - char(0xff))
			continue;
		if (*(data - 5) - char(0x00))
			continue;

		CHECK(players.size() > index, index, players.size());

		const int amount = _byteswap_ulong(*reinterpret_cast<const int*>(data + 1));	
		players[index++].Stack(amount);
	}

	for (const Player& p : players)
		LOG_TRACE("Player: '%s', stack: '%s'") % p.Name() % p.Stack();

	table.PlayersInfo(players);
}

} // namespace msg
} // namespace ps
} // namespace clnt