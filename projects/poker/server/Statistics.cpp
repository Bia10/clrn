#include "Statistics.h"
#include "SQLiteDB.h"
#include "FileSystem.h"
#include "Exception.h"

#include <boost/thread/mutex.hpp>

namespace srv
{
const char SQL_INSERT_FLOP[] =
	"INSERT INTO flops "
				"(first, "
				 "second, "
				 "third, "
				 "fourth, "
				 "fifth) "
	"VALUES     (?, ?, ?, ?, ?)";

const char SQL_INSERT_GAME[] = 
	"INSERT INTO games "
		"(players, flop) "
	"VALUES(?, ?)";

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
				 "first_card, "
				 "second_card) " 
	"VALUES      (?, ?, ?, ?) ";

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

class Statistics::Impl
{
public:

	class ScopedTransaction
	{
	public:
		ScopedTransaction(sql::IDatabase& db) : m_DB(db), m_Done(false), m_Lock(m_Mutex)
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
		boost::mutex m_Mutex;
		boost::mutex::scoped_lock m_Lock;
	};

Impl(ILog& logger) : m_Log(logger), m_DB(new sql::SQLiteDataBase(m_Log))
{
	TRY 
	{
		SCOPED_LOG(m_Log);
		const std::string path = fs::FullPath("stat.db");
		CHECK(fs::Exists(path), "Failed to find database", path);
		m_DB->Open(path);
	}
	CATCH_PASS_EXCEPTIONS("Failed to init statistics")
}

void Write(Parser::Data& data)
{
	TRY 
	{
		SCOPED_LOG(m_Log);

		// scoped transaction
		ScopedTransaction transaction(*m_DB);

		sql::IStatement::Ptr statement;
		unsigned int flopId = 0;

		// write flop first
		if (!data.m_Flop.empty())
		{
			statement = m_DB->CreateStatement(SQL_INSERT_FLOP);
			for (int i = 0 ; i < 5 ; ++i)
			{
				if (data.m_Flop.size() > 1)
					*statement << data.m_Flop[i];
				else
					*statement << sql::Null();
			}
			statement->Execute();
			flopId = static_cast<unsigned int>(m_DB->LastRowId());
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
		statement = m_DB->CreateStatement(SQL_INSERT_PLAYERS);
		for (Parser::Data::Player& player : data.m_Players)
		{
			// check and insert name
			*statement << player.m_Name << player.m_Name;

			// get player id
			player.m_Index = static_cast<unsigned int>(m_DB->LastRowId());

			statement->Execute();
		}

		// insert player percents
		statement = m_DB->CreateStatement(SQL_INSERT_PERCENTS);
		for (const Parser::Data::Player& player : data.m_Players)
		{
			for (std::size_t street = 0 ; street < player.m_Percents.size(); ++street)
			{
				*statement << player.m_Index << gameId << street << player.m_Percents[street];
				statement->Execute();
			}
		}

		// insert hands
		statement = m_DB->CreateStatement(SQL_INSERT_HANDS);
		for (const Parser::Data::Hand& hand : data.m_Hands)
		{
			*statement << data.m_Players[hand.m_PlayerIndex].m_Index << gameId << hand.m_First << hand.m_Second;
			statement->Execute();
		}

		// insert actions
		statement = m_DB->CreateStatement(SQL_INSERT_ACTIONS);
		for (const Parser::Data::Action& action : data.m_Actions)
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
	CATCH_PASS_EXCEPTIONS("Failed to write data to database")
}

private:
	ILog& m_Log;
	sql::IDatabase::Ptr m_DB;
};

Statistics::Statistics(ILog& logger) : m_Impl(new Impl(logger))
{

}

Statistics::~Statistics()
{
	delete m_Impl;
}

void Statistics::Write(Parser::Data& data)
{
	m_Impl->Write(data);
}

}