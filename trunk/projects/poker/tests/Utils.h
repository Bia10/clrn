#ifndef Utils_h__
#define Utils_h__

#include "Cards.h"
#include "ITable.h"

#include <string>


namespace tests
{

    void ParseCards(const std::string& text, pcmn::Card::List& result);
    void ParseData(const std::string& data, clnt::ITable& table);
}

#endif // Utils_h__
