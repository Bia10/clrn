#include "PokerStarsConfig.h"
#include "IniParser.h"
#include "Conversion.h"
#include "Exception.h"

#include <sstream>
#include <iomanip>

#include <boost/filesystem.hpp>

namespace clnt
{
namespace ps
{
    unsigned ParseValue(const std::string& key)
    {
        CHECK(!key.empty());
        CHECK(key.front() == 'K', "Invalid key in ini file", key);

        unsigned result = 0;
        {
            std::stringstream ss;
            ss << std::hex << conv::cast<unsigned>(key.substr(1));
            result = conv::cast<unsigned>(ss.str()) / 10000;
            CHECK(!ss.fail(), "Failed to convert hex key to dec value", key); 
        }

        {
            std::stringstream ss;
            ss << conv::cast<std::string>(result);
            ss >> std::hex >> result;
            CHECK(!ss.fail(), "Failed to convert key to hex value", key); 
        }

        return result;
    }

    std::string ValueToConfig(const unsigned value)
    {
        unsigned hexValue = 0;
        {
            std::stringstream ss;
            ss << std::hex << value;
            hexValue = conv::cast<unsigned>(ss.str()) * 10000;
            CHECK(!ss.fail(), "Failed to convert hex key to dec value", value); 
        }

        std::stringstream ss;
        ss << conv::cast<std::string>(hexValue);
        unsigned res = 0;
        ss >> std::hex >> res;
        CHECK(!ss.fail(), "Failed to convert key to hex value", value); 
        return std::string("K") + conv::cast<std::string>(res);
    }

    Config::Config(ILog& logger) : m_Log(logger)
    {

    }

    void Config::Read() 
    {
        TRY 
        {
	        m_Data = Settings();
	
	        const std::string directory = boost::filesystem::system_complete(boost::filesystem::temp_directory_path().string() + "../PokerStars/").string();
	        const std::string file = directory + "user.ini";
	
	        INI<> parser(file, false);
	        CHECK(parser.Parse(), "Failed to parse ini file", file);
	        parser.Select("HotKeys");
	
	        for (const auto pair : *parser.current)
	        {
	            const std::string& value = pair.second;
	            const std::string::size_type delim = value.rfind(',');
	
	            if (delim != std::string::npos)
	            {
	                const std::string keyVal = value.substr(0, delim);
	
	                if (keyVal == "33554432")
	                    m_Data.m_FoldButton = ParseValue(pair.first);
	                else
	                if (keyVal == "83886080")
	                    m_Data.m_CheckCallButton = ParseValue(pair.first);
	                else
	                if (keyVal == "100663296")
	                    m_Data.m_RaiseButton = ParseValue(pair.first);
	            }
	        }

            if (!m_Data.m_FoldButton)
                m_Data.m_FoldButton = 61; // num 1
            if (!m_Data.m_CheckCallButton)
                m_Data.m_CheckCallButton = 62; // num 2
            if (!m_Data.m_RaiseButton)
                m_Data.m_RaiseButton = 63; // num 3

            Write();
        }
        CATCH_PASS_EXCEPTIONS("Failed to read settings")
    }

    void Config::Write() const
    {
        TRY 
        {
	        const std::string directory = boost::filesystem::system_complete(boost::filesystem::temp_directory_path().string() + "../PokerStars/").string();
	        const std::string file = directory + "user.ini";
	
	        INI<> parser(file, false);
	        CHECK(parser.Parse(), "Failed to parse ini file", file);

            parser.Select("HotKeys");
            parser.current->clear();

            parser["HotKeys"][ValueToConfig(m_Data.m_FoldButton)] = "33554432,1";
            parser["HotKeys"][ValueToConfig(m_Data.m_CheckCallButton)] = "83886080,1";
            parser["HotKeys"][ValueToConfig(m_Data.m_RaiseButton)] = "100663296,1";

            parser.Save(file);
        }
        CATCH_PASS_EXCEPTIONS("Failed to write settings")
    }

    const IConfig::Settings& Config::Get() const 
    {
        return m_Data;
    }

    void Config::Set(const Settings& data)
    {
        m_Data = data;
    }

}
}