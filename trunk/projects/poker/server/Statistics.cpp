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
			else
				m_DB.Commit();
		}

		void Done()	{ m_Done = true; }

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

		transaction.Done();
	}
	CATCH_IGNORE_EXCEPTIONS(m_Log, "Failed to write data to database")
}


unsigned int InsertCards(const std::vector<int>& cards)
{
	TRY 
	{
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

}