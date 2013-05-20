#include "Log.h"
#include "IniParser.h"
#include "../clientlib/PokerStarsTableControl.h"
#include "../clientlib/PokerStarsConfig.h"

#include "gtest/gtest.h"

#include <iostream>

#include <boost/asio.hpp>

using testing::Range;
using testing::Combine;

Log g_Log;

void DoControlTest()
{
    boost::asio::io_service io;

    clnt::ps::Config cfg(g_Log);

    clnt::ps::TableControl control(g_Log, io, cfg, HWND(0x00190958));
    control.Fold();

    io.run();
}

TEST(TableControl, PokerStars)
{
    DoControlTest();
}