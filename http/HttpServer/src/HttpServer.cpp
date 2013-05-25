
#include "Server.h"
#include "Exception.h"
#include "Log.h"
#include "ServerLogger.h"
#include "ServerSettings.h"

#include <google/profiler.h>

#include <iomanip>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "Crypto.h"
#include "Converter.h"

int main(int argc, char* argv[])
{
	try
	{
		/*
		cmn::CCrypto Crypto(_T("key"));

		std::vector< unsigned char > vecTemp;
		Crypto.Encrypt(_T("wasd"), vecTemp);

		std::cout << &vecTemp.front() << std::endl;

		_tostringstream ossResult;
		for (const auto& cSymbol : vecTemp)
		{
			ossResult << std::setfill('0') << std::setw(2) << std::hex << static_cast< int > (cSymbol);
			std::cout << ossResult.str() << std::endl;
		}

		_tstring sTemp(vecTemp.begin(), vecTemp.end());
		Crypto.Decrypt(sTemp, vecTemp);

		std::cout << &vecTemp.front() << std::endl;

*/
		if (argc != 2)
		{
			std::cout << "Usage: HttpServer <settings_file_name>" << std::endl;
			return 0;
		}

		//ProfilerStart("profile_data.txt");

		cmn::Settings::Open(argv[1]);

		const _tstring sLogSource 		= cmn::Settings::Instance().GetLogSource();
		const cmn::ILog::Level eLevel 	= cmn::Settings::Instance().GetLogLevel();

		cmn::CLog Logger;
		if (sLogSource == _T("std::cout"))
		{
			Logger.Open(std::cout, eLevel);
		}
		else
		{
			Logger.Open(sLogSource, eLevel);
		}

		cmn::Logger::Init(&Logger);

		const _tstring sHost 		= cmn::Settings::Instance().GetHost();
		const std::size_t nPort 	= cmn::Settings::Instance().GetPort();
		const std::size_t nThreads 	= cmn::Settings::Instance().GetNumberOfThreads();

		srv::CServer Srv(Logger, sHost, nPort, nThreads);

		Srv.Init();
		Srv.Run();
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
	catch(...)
	{
		std::cout << _T("Unhandled exception.") << std::endl;
	}

	/*ProfilerFlush();
	ProfilerStop();
*/
}
