#include "Messages.h"
#include "Modules.h"
#include "PokerStars.h"
#include "Actions.h"
#include "ITable.h"
#include "Cards.h"
#include "Exception.h"
#include "PacketParser.h"

#include <sstream>
#include <iomanip>

#include <boost/make_shared.hpp>
#include <boost/assign.hpp>

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
	case 'f' : return Action::ShowCards; // show cards before fold
	case 'z' : return Action::Fold;	// show cards and fold
	case 'A' : return Action::Ante;	
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

unsigned ParseAnte(const dasm::WindowMessage& message)
{
	static const std::string regexp = "0\\d'\\s'\\s41'A'\\s00'\\s'\\s00'\\s'\\s(..'.'\\s..'.')\\s";

	std::ostringstream oss;
	BytesToString(oss, message.m_Block.m_Data + message.m_Block.m_Offset, message.m_Block.m_Offset + message.m_Block.m_Size);

	PacketParser parser(regexp, oss.str());

	if (parser.Eof())
		return 0;

	return parser.GetInt(1);
}

void ParsePlayers(const dasm::WindowMessage& message, pcmn::Player::List& players)
{
	static const std::string regexp("ff'ÿ'\\s00'\\s'\\s00'\\s'\\s00'\\s'\\s1c'\\s'((\\s..'.'){4})\\s00'\\s'\\s00'\\s'(\\s..'.'){8}(\\s00'\\s'){3}\\s..'.'\\sff'ÿ'(\\s..'.'){9}\\sff'ÿ'(\\s00'\\s'){3}\\s..'.'(\\s00'\\s'){2}(\\s..'.'){2}\\sff'ÿ'(\\s00'\\s'){2}(\\s..'.'){2}\\s(([2-7][0-9a-f]'[\\w\\s]'\\s)*)00'\\s'");

	std::ostringstream oss;
	BytesToString(oss, message.m_Block.m_Data + message.m_Block.m_Offset, message.m_Block.m_Offset + message.m_Block.m_Size);

	PacketParser parser(regexp, oss.str());

	for (; !parser.Eof(); ++parser)
	{
		const std::string name = parser.GetString(11);
		if (name.empty())
			continue;

		const unsigned amount = parser.GetInt(1);

		players.push_back(boost::make_shared<Player>(name, amount));
	}
}

void TableInfo::ParseActions(const dasm::WindowMessage& message, ITable& table) const
{
	static const std::string regexp("(([2-7][0-9a-f]'[\\w\\s]'\\s)*)00'\\s'(\\s..'.'){5}\\sff'ÿ'(\\s00'\\s'){4}(\\s..'.'){4}\\sff'ÿ'(\\s..'.'){8}\\sff'ÿ'(\\s..'.'){19}\\s\\d\\d'(\\w)'(\\s..'.'){4}\\s");

	std::ostringstream oss;
	BytesToString(oss, message.m_Block.m_Data + message.m_Block.m_Offset, message.m_Block.m_Offset + message.m_Block.m_Size);

	PacketParser parser(regexp, oss.str());

	for (; !parser.Eof(); ++parser)
	{
		const std::string name = parser.GetString(1);
		const std::string action = parser.GetString(8);
		const unsigned amount = parser.GetInt(9);
		
		if (name.size() < 3 || action.size() != 1)
			continue;

		const pcmn::Action::Value actionValue = ConvertAction(action.front());

		if (actionValue == pcmn::Action::Ante)
		{
			table.Ante(amount);
		}
		else
		{
			if (actionValue == Action::Unknown)
			{
				LOG_TRACE("Unknown action: '%s', player: '%s', amount: '%s'") % action % name % amount;
				continue;
			}

			try
			{
				table.PlayerAction(name, actionValue, amount);
			}
			catch (const std::exception& e)
			{
				LOG_TRACE("Failed to add action: '%s', player: '%s', amount: '%s'. Exception: %s") % action % name % amount % e.what();
			}
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

	Action::Value actionValue = ConvertAction(action);

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
		LOG_TRACE("Player: '%s', action: '%p', amount: '%s'") % name % Action::ToString(actionValue) % amount;

		CHECK(actionValue != Action::Unknown, static_cast<int>(action));
		if (!name.empty())
			table.PlayerAction(name, actionValue, amount);
	}

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

		const int stack = _byteswap_ulong(*reinterpret_cast<const int*>(data + 1));	

		LOG_TRACE("Player: '%s', stack: '%s'") % name % stack;
		table.PlayersInfo(boost::assign::list_of(boost::make_shared<pcmn::Player>(name, stack)));
	}
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

	if (!first.IsValid() || !second.IsValid())
		return;

	if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Trace))
	{
		std::ostringstream oss;
		oss << "('" << Card::ToString(first.m_Value) << "' of '" << Suit::ToString(first.m_Suit) << "')";
		oss << "('" << Card::ToString(second.m_Value) << "' of '" << Suit::ToString(second.m_Suit) << "')";

		LOG_TRACE("Player cards: %s") % oss.str();
	}

	table.BotCards(first, second);
}


std::size_t CashGameInfo::GetId() const 
{
	return 0x20000;
}

void CashGameInfo::Process(const dasm::WindowMessage& message, ITable& table) const 
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

		const Player::Ptr player = boost::make_shared<Player>(name, stack);
		player->Country(country);

		players.push_back(player);
	}

	table.PlayersInfo(players);
}


std::size_t BeforePreflopActions::GetId() const 
{
	return 0x1006;
}

void BeforePreflopActions::Process(const dasm::WindowMessage& message, ITable& table) const 
{
	dasm::MessageRecord record(message);

	const char* data = message.m_Block.m_Data + message.m_Block.m_Offset;
	const char* end = message.m_Block.m_Data + message.m_Block.m_Offset + message.m_Block.m_Size;

	Player::List players;

	for (int i = 0 ; i < 5 && data < end; ++i)
		data = std::find(data + 1, end, 0xff);

	ITable::Actions actions;
	while (data < end)
	{
		data += 0x14;

		const Action::Value actionValue = ConvertAction(*data);
		++data;
		const unsigned amount = _byteswap_ulong(*reinterpret_cast<const int*>(data));	
		data += sizeof(unsigned int);

		if (*data < 0x20)
			break;

		const std::string name(data);

		if (name.size() < 3)
			continue;

		if (actionValue != pcmn::Action::Unknown)
			actions.push_back(ITable::ActionDesc(name, actionValue, amount));

		players.push_back(boost::make_shared<Player>(name, 0));
	
		for (int i = 0 ; i < 3 && data < end; ++i)
			data = std::find(data + 1, end, 0xff);
	}

	if (players.empty())
		return;

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

		const int amount = _byteswap_ulong(*reinterpret_cast<const int*>(data + 1));	

		if (index == players.size())
			break;

		players[index++]->Stack(amount);
	}

	const unsigned ante = ParseAnte(message);

	for (const Player::Ptr& p : players)
		LOG_TRACE("Player: '%s', stack: '%s'") % p->Name() % p->Stack();

	LOG_TRACE("Ante: '%s'") % ante;

	table.PlayersInfo(players);
	table.Ante(ante);

	for (const ITable::ActionDesc& action : actions)
	{
		LOG_TRACE("Player: '%s', action: '%p', amount: '%s'") % action.m_Name % Action::ToString(action.m_Value) % action.m_Amount;
		table.PlayerAction(action.m_Name, action.m_Value, action.m_Amount);
	}
}


std::size_t TableInfo::GetId() const 
{
	return 0x0000;
}

void TableInfo::Process(const dasm::WindowMessage& message, ITable& table) const 
{
	const unsigned ante = ParseAnte(message);

	LOG_TRACE("Ante: '%s'") % ante;
	table.Ante(ante);

	pcmn::Player::List players;
	ParsePlayers(message, players);

	for (const Player::Ptr& p : players)
		LOG_TRACE("Player: '%s', stack: '%s'") % p->Name() % p->Stack();

	table.PlayersInfo(players);

	//ParseActions(message, table);
}

} // namespace msg
} // namespace ps
} // namespace clnt