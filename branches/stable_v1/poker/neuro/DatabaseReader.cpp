#include "DatabaseReader.h"
#include "Exception.h"
#include "Config.h"
#include "SQLiteDB.h"
#include "Params.h"

#include <boost/format.hpp>
#include <boost/thread/mutex.hpp>

namespace neuro
{
	DatabaseReader::DatabaseReader(ILog& logger, const std::string& file) : m_Db(new sql::SQLiteDataBase(logger))
	{
        m_Db->Open(file);
	}

	DatabaseReader::~DatabaseReader()
	{
		delete m_Db;
	}

	void DatabaseReader::Process(const std::vector<float>& input, std::vector<float>& output)
	{
        static boost::mutex mx;
        boost::mutex::scoped_lock lock(mx);

		CHECK(input.size() == cfg::INPUT_COUNT);
        output.clear();

        Params params;
        params.FromNeuroFormat(input, output);

        const std::string where
        (
            (
                boost::format
                (
                    " win = %s "
                    "AND position = %s "
                    "AND bet = %s "
                    "AND players = %s "
                    "AND danger = %s "
                    "AND bot_avg_style = %s "
                    "AND bot_style = %s "
                    "AND bot_stack = %s "
                ) 
                % params.m_WinRate
                % params.m_Position
                % params.m_BetSize
                % params.m_ActivePlayers
                % params.m_Danger
                % params.m_BotAverageStyle
                % params.m_BotStyle
                % params.m_BotStackSize
            ).str()
        );

        Params::List result;
        Params::ReadAll(result, *m_Db, where);

        CHECK(result.size() == 1, "Invalid result size for query", result.size(), where);

        output.push_back(static_cast<float>(result.front().m_CheckFold));
        output.push_back(static_cast<float>(result.front().m_CheckCall));
        output.push_back(static_cast<float>(result.front().m_BetRaise));
	}

} // namespace neuro


