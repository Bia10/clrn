#include "DatabaseWriter.h"
#include "Exception.h"
#include "Config.h"
#include "SQLiteDB.h"

#include "Params.h"

namespace neuro
{
	DatabaseWriter::DatabaseWriter(ILog& logger, const std::string& file) : m_Db(new sql::SQLiteDataBase(logger))
	{
        m_Db->Open(file);
	}

	DatabaseWriter::~DatabaseWriter()
	{
		delete m_Db;
	}

	void DatabaseWriter::Process(const std::vector<float>& input, std::vector<float>& output)
	{
        CHECK(input.size() == cfg::INPUT_COUNT);
        CHECK(output.size() == cfg::OUTPUT_COUNT);

        Params::List params(1);
        params.back().FromNeuroFormat(input, output);
        Params::WriteAll(params, *m_Db);
	}

} // namespace neuro


