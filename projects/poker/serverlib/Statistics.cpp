#include "Statistics.h"
#include "SQLiteDB.h"
#include "FileSystem.h"
#include "Exception.h"
#include "Config.h"

#include <boost/thread/mutex.hpp>
#include <boost/format.hpp>

namespace srv
{

const char SQL_SELECT_MAX_FLOP[] =
	"SELECT Max(id) "
	"FROM cards";

const char SQL_INSERT_FLOP[] =
	"INSERT INTO cards "
				"(id, value) "
	"VALUES     (?, ?)";

const char SQL_INSERT_GAME[] = 
	"INSERT INTO games "
		"(players, flop) "
	"VALUES(?, ?)";

const char SQL_GET_PLAYER[] = 
	"SELECT id "
		"FROM players "
	"WHERE name = '%s'";

const char SQL_INSERT_PLAYERS[] = 
	"INSERT INTO players "
				"(name) "
	"SELECT ? "
	"WHERE  NOT EXISTS (SELECT 1 "
					   "FROM   players "
					   "WHERE  name = ?); ";

const char SQL_INSERT_PERCENTS[] = 
	"INSERT INTO percents "
				"(player, "
				 "game, "
				 "street, "
				 "value) " 
	"VALUES      (?, ?, ?, ?) ";

const char SQL_INSERT_HANDS[] = 
	"INSERT INTO hands "
				"(player, "
				 "game, "
				 "cards) " 
	"VALUES      (?, ?, ?) ";

const char SQL_INSERT_ACTIONS[] = 
	"INSERT INTO actions "
				"(game, "
				 "player, "
				 "street, "
				 "action, "
				 "pot_amount, "
				 "stack_amount, "
				 "position) "
	"VALUES     (?, ?, ?, ?, ?, ?, ?) ";

const char SQL_GET_CARD_RANGES[] = 
	"SELECT p.name name, "
		   "( Sum(c.value) * 3 ) / ( Count(c.value) * 2 ) average "
	"FROM   actions a "
		   "JOIN players p "
			 "ON a.player = p.id "
		   "JOIN hands h "
			 "ON h.game = a.game "
				"AND h.player = p.id "
		   "JOIN cards c "
			 "ON c.id = h.cards "
	"WHERE  a.street = 0 "
		   "AND (%s) "
	"GROUP  BY p.id, "
			  "a.action "
	"HAVING Count(c.value) > 2 ";

const char SQL_GET_LAST_ACTIONS[] = 
	"SELECT a1.action, "
		   "Count(*) "
	"FROM   actions a1 "
		   "JOIN players p1 "      
			 "ON a1.player = p1.id "
	"WHERE  p1.name = '%s' "
		   "AND a1.action BETWEEN 0 AND 4 "
		   "AND EXISTS (SELECT 1 "
					   "FROM   actions a2 "
							  "JOIN players p2 "
								"ON a2.player = p2.id "
					   "WHERE  a2.game = a1.game "
							  "AND p2.name = '%s') "                                                    
	"GROUP  BY a1.game, "
			  "a1.action "
	"ORDER  BY 1 desc "
	"limit 20 ";

const char SQL_GET_PLAYER_EQUITIES[] = 
	"SELECT p.name        name, "
		   "Avg(pc.value) rate "
	"FROM   actions a "
		   "JOIN players p "
			 "ON a.player = p.id "
		   "JOIN percents pc "
			 "ON pc.game = a.game "
				"AND pc.player = a.player "
				"AND pc.street = a.street "
	"WHERE  (%s) "
	"GROUP  BY p.id "
	"HAVING Count(DISTINCT a.game) > 2 ";


static const int CURRENT_MODULE_ID = Modules::Server;

class Statistics::Impl
{
public:

	class ScopedTransaction
	{
	public:
		ScopedTransaction(sql::IDatabase& db, boost::mutex& mutex) : m_DB(db), m_Done(false), m_Lock(mutex)
		{
			m_DB.BeginTransaction();
		}

		~ScopedTransaction()
		{
			if (!m_Done)
				m_DB.Rollback();
		}

		void Commit()	
		{ 
            m_Done = true; 
			m_DB.Commit();
		}

	private:
		bool m_Done;
		sql::IDatabase& m_DB;
		boost::mutex::scoped_lock m_Lock;
	};

Impl(ILog& logger) : m_Log(logger), m_DB(new sql::SQLiteDataBase(m_Log))
{
	TRY 
	{
		SCOPED_LOG(m_Log);
		const std::string path = fs::FullPath(cfg::DB_FILE_NAME);
		CHECK(fs::Exists(path), "Failed to find database", path);
		m_DB->Open(path);
	}
	CATCH_PASS_EXCEPTIONS("Failed to init statistics")
}

void Write(pcmn::TableContext::Data& data)
{
	TRY 
	{
		SCOPED_LOG(m_Log);

		// scoped transaction
		ScopedTransaction transaction(*m_DB, m_Mutex);

		sql::IStatement::Ptr statement;
		unsigned int flopId = 0;

		// write flop first
		if (!data.m_Flop.empty())
		{
			// this flop id
			flopId = InsertCards(data.m_Flop);
		}

		// write game
		statement = m_DB->CreateStatement(SQL_INSERT_GAME);
		*statement << data.m_Players.size();
		if (flopId)
			*statement << flopId;
		else
			*statement << sql::Null();

		statement->Execute();

		// game rowid
		const unsigned int gameId = static_cast<unsigned int>(m_DB->LastRowId());

		// write all players
		for (pcmn::TableContext::Data::Player& player : data.m_Players)
		{
			// check and insert name
			player.m_Index = InsertPlayer(player.m_Name);
		}

		// insert player percents
		statement = m_DB->CreateStatement(SQL_INSERT_PERCENTS);
		for (const pcmn::TableContext::Data::Player& player : data.m_Players)
		{
			for (std::size_t street = 0 ; street < player.m_Percents.size(); ++street)
			{
				*statement << player.m_Index << gameId << street << player.m_Percents[street];
				statement->Execute();
			}
		}

		// insert hands
		statement = m_DB->CreateStatement(SQL_INSERT_HANDS);
		for (const pcmn::TableContext::Data::Hand& hand : data.m_Hands)
		{
			const unsigned int cardsId = InsertCards(hand.m_Cards);
			*statement << data.m_Players[hand.m_PlayerIndex].m_Index << gameId << cardsId;
			statement->Execute();
		}

		// insert actions
		statement = m_DB->CreateStatement(SQL_INSERT_ACTIONS);
		for (const pcmn::TableContext::Data::Action& action : data.m_Actions)
		{
			*statement
				<< gameId
				<< data.m_Players[action.m_PlayerIndex].m_Index
				<< action.m_Street
				<< action.m_Action
				<< action.m_PotAmount
				<< action.m_StackAmount
				<< action.m_Position;

			statement->Execute();
		}

		transaction.Commit();
	}
	CATCH_IGNORE_EXCEPTIONS(m_Log, "Failed to write data to database")
}


unsigned int InsertCards(const std::vector<int>& cards)
{
	TRY 
	{
        SCOPED_LOG(m_Log);

		// this flop id
		const unsigned int result = m_DB->ExecuteScalar(SQL_SELECT_MAX_FLOP) + 1;
	
		const sql::IStatement::Ptr statement = m_DB->CreateStatement(SQL_INSERT_FLOP);
		for (const int card : cards)
		{
			*statement << result << card;
			statement->Execute();
		}
	
		return result;
	}
	CATCH_PASS_EXCEPTIONS("InsertCards failed")
}

unsigned int InsertPlayer(const std::string& name)
{
	TRY 
	{
        SCOPED_LOG(m_Log);

		const std::map<std::string, unsigned int>::const_iterator it = m_Players.find(name);
		if (it != m_Players.end())
			return it->second; // player already inserted

		unsigned int result = 0;
		const sql::IStatement::Ptr insertPlayer = m_DB->CreateStatement(SQL_INSERT_PLAYERS);
		*insertPlayer << name << name;
		if (insertPlayer->Execute())
		{
			result = static_cast<unsigned int>(m_DB->LastRowId());
		}
		else
		{
			const sql::Recordset::Ptr recordset = m_DB->Fetch((boost::format(SQL_GET_PLAYER) % name).str());
			assert(!recordset->Eof());
			result = recordset->Get<unsigned int>(0);
		}

		m_Players.insert(std::make_pair(name, result));
		return result;
	}
	CATCH_PASS_EXCEPTIONS("InsertPlayer failed", name)
}

unsigned GetRanges(PlayerInfo::List& players)
{
	TRY
	{
        SCOPED_LOG(m_Log);

		std::map<std::string, unsigned> indexes;
		const std::string filter = GetPlayersFilter(players, indexes);

		const std::string sql = (boost::format(SQL_GET_CARD_RANGES) % filter).str();

		const sql::Recordset::Ptr recordset = m_DB->Fetch(sql);
		unsigned count = 0;
		while (!recordset->Eof())
		{
			const std::string name = recordset->Get<std::string>(0);
			const int range = recordset->Get<int>(1);

			const unsigned index = indexes[name];
			players[index].m_CardRange = range;

            LOG_TRACE("Player: [%s], range: [%s]") % players[index].m_Name % range;

			++(*recordset);
			++count;
		}
		return count;
	}
	CATCH_PASS_EXCEPTIONS("GetRanges failed")
}

void GetLastActions(const std::string& target, const std::string& opponent, int& checkFolds, int& calls, int& raises)
{
	TRY
	{
        SCOPED_LOG(m_Log);

		checkFolds = 0;
		calls = 0;
		raises = 0;

		const std::string sql = (boost::format(SQL_GET_LAST_ACTIONS) % target % opponent).str();

		const sql::Recordset::Ptr recordset = m_DB->Fetch(sql);
		while (!recordset->Eof())
		{
			const pcmn::Action::Value action = static_cast<pcmn::Action::Value>(recordset->Get<int>(0));
			const int count = recordset->Get<int>(1);

            LOG_TRACE("Last actions with: [%s], action: [%s], count: [%s]") % opponent % pcmn::Action::ToString(action) % count;

			switch (action)
			{
			case pcmn::Action::Check:
			case pcmn::Action::Fold:
				checkFolds += count;
				break;
			case pcmn::Action::Call:
				calls += count;
				break;
			case pcmn::Action::Bet:
			case pcmn::Action::Raise:
				raises += count;
				break;
			}

			++(*recordset);
		}
	}
	CATCH_PASS_EXCEPTIONS("GetLastActions failed")
}

unsigned GetEquities(PlayerInfo::List& players)
{
	TRY
	{
        SCOPED_LOG(m_Log);

		std::map<std::string, unsigned> indexes;
		const std::string filter = GetPlayersFilter(players, indexes);

		const std::string sql = (boost::format(SQL_GET_PLAYER_EQUITIES) % filter).str();

		const sql::Recordset::Ptr recordset = m_DB->Fetch(sql);
		unsigned count = 0;
		while (!recordset->Eof())
		{
			const std::string name = recordset->Get<std::string>(0);
			const double rate = recordset->Get<double>(1);

			const unsigned index = indexes[name];
			players[index].m_WinRate = static_cast<float>(rate);
			
            LOG_TRACE("Player: [%s], win rate: [%s]") % name % rate;

			++(*recordset);
			++count;
		}

		return count;
	}
	CATCH_PASS_EXCEPTIONS("GetEquities failed")
}

private:

std::string GetActionsFilter(const PlayerInfo::Actions& actions)
{
	std::ostringstream oss;
	for (unsigned i = 0 ; i < actions.size(); ++i)
	{
		if (i)
			oss << ", ";
		oss << actions[i];
	}
	return oss.str();
}

std::string GetPlayersFilter(PlayerInfo::List& players, std::map<std::string, unsigned>& indexes)
{
	std::ostringstream oss;
	int count = 0;
	for (const PlayerInfo& player : players)
	{
		if (count)
			oss << " OR ";
		oss 
			<< "(a.pot_amount > " << player.m_PotAmount << " "
			<< "AND a.action IN (" << GetActionsFilter(player.m_Actions) << ") "
			<< "AND p.name = '" << player.m_Name << "' )";

		indexes[player.m_Name] = count;
		++count;
	}
	return oss.str();
}

private:
	ILog& m_Log;
	sql::IDatabase::Ptr m_DB;
	boost::mutex m_Mutex;
	std::map<std::string, unsigned int> m_Players;
};

Statistics::Statistics(ILog& logger) : m_Impl(new Impl(logger))
{

}

Statistics::~Statistics()
{
	delete m_Impl;
}

void Statistics::Write(pcmn::TableContext::Data& data)
{
	m_Impl->Write(data);
}

unsigned Statistics::GetRanges(PlayerInfo::List& players) const
{
	return m_Impl->GetRanges(players);
}

void Statistics::GetLastActions(const std::string& target, const std::string& opponent, int& checkFolds, int& calls, int& raises) const
{
	m_Impl->GetLastActions(target, opponent, checkFolds, calls, raises);
}

unsigned Statistics::GetEquities(PlayerInfo::List& players) const
{
	return m_Impl->GetEquities(players);
}

}

