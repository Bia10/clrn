#include "stdafx.h"
#include "Compiler.h"

namespace po = boost::program_options;

int main(int ac, char* av[])
{
	Log logger;
	logger.Open("1", Modules::Tools, ILog::Level::Trace);

	TRY 
	{
		// Declare the supported options.
		po::options_description desc("Allowed options");
		desc.add_options()
			("help", "produce help message")
			("db-file,D", po::value<std::string>(), "set database file name")
			("create-new,C", po::value<bool>(), "create new database")
			("script-path,I", po::value<std::string>(), "script path")
			("script-file", po::value< std::vector<std::string> >(), "input script file");

		po::positional_options_description p;
		p.add("script-file", -1);
		 
		po::variables_map vm;
		po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);
		po::notify(vm);

		if (vm.count("help")) 
		{
			std::cout << desc << std::endl;
			return 1;
		}

		CDBCompiler compiler(logger);

		const std::string databaseFileName = vm["db-file"].as<std::string>();

		const bool create = vm["create-new"].as<bool>();
		if (create)		
			compiler.Create(databaseFileName);
		else
			compiler.Open(databaseFileName);

		const std::string scriptPath = vm["script-path"].as<std::string>();

		const std::vector<std::string> files = vm["script-file"].as<std::vector<std::string> >();
		BOOST_FOREACH(const std::string& file, files)
		{
			boost::filesystem::path path = boost::filesystem::system_complete(scriptPath);
			path.append(file.begin(), file.end());
			CHECK(boost::filesystem::exists(path), path.string());

			const std::string full = boost::filesystem::system_complete(path).string();

			logger.Trace(CURRENT_MODULE_ID, __FUNCTION__, "Executing: [%s]") % full;

			compiler.ExecuteFile(full);
		}
	}
	CATCH_IGNORE_EXCEPTIONS(logger, "main failed.")

	logger.Close();

	return 0;
}

