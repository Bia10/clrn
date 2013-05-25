#include "Log.h"
#include "SQLiteDB.h"
#include "FileSystem.h"
#include "CppSQLite3.h"

#include "gtest/gtest.h"

#include <iostream>

#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

Log g_Logger;

TEST(SQLite, InsertFetch)
{
	fs::Remove("test.db");
	sql::SQLiteDataBase db(g_Logger);
	db.Open("test.db");

	db.BeginTransaction();
	db.Execute("create table emp(empno int, empname char(20));");

	const sql::IStatement::Ptr stmnt = db.CreateStatement("insert into emp values(?, ?)");

	const std::size_t rows = 100000;
	for (std::size_t i = 0 ; i < rows; ++i)
	{
		const std::string name = std::string("employee_") + boost::lexical_cast<std::string>(i);
		*stmnt << int(i) << name;
		stmnt->Execute();
	}

	db.Commit();

	ASSERT_EQ(rows, db.ExecuteScalar("select count(*) from emp"));

	const sql::Recordset::Ptr recordset = db.Fetch("select * from emp");
	int row = 0;
	while (!recordset->Eof())
	{
		const int value = recordset->Get<int>(0);
		ASSERT_EQ(value, row);
		++(*recordset);
		++row;
	}
}


void ReadThread()
{
    boost::this_thread::interruptible_wait(100);
	sql::SQLiteDataBase db(g_Logger);
	db.Open("test.db");

	for (;;)
	{
		try
		{
			const sql::Recordset::Ptr recordset = db.Fetch("select * from emp");
			int row = 0;
			while (!recordset->Eof())
			{
				const int value = recordset->Get<int>(0);
				//ASSERT_EQ(value, row);
				++(*recordset);
				++row;
			}

			std::cout << "Read : " << row << std::endl;

			boost::this_thread::interruption_point();
		}
		catch (const std::exception& e)
		{
            FAIL() << e.what();
			std::cout << e.what() << std::endl;
		}
	}
}

void WriteThread()
{
	sql::SQLiteDataBase db(g_Logger);
	db.Open("test.db");

	db.Execute("create table emp(empno int, empname char(20));");
	std::size_t i = 0;
	std::size_t counter = 0;

	for (;;)
	{
		try
		{
			db.BeginTransaction();

			const sql::IStatement::Ptr stmnt = db.CreateStatement("insert into emp values(?, ?)");

			const std::size_t rows = 1000;
			for (std::size_t i = 0 ; i < rows; ++i)
			{
				const std::string name = std::string("employee_") + boost::lexical_cast<std::string>(counter);
				*stmnt << int(counter++) << name;
				stmnt->Execute();
			}

			db.Commit();
			boost::this_thread::interruption_point();
		}
		catch (const std::exception& e)
		{
            FAIL() << e.what();
			std::cout << e.what() << std::endl;
		}
	}
}

TEST(SQLite, Mulithread)
{
	fs::Remove("test.db");

	boost::thread read(&ReadThread);
	boost::thread write(&WriteThread);

	boost::this_thread::interruptible_wait(10000);

	read.interrupt();
	write.interrupt();
}