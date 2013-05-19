#include "Log.h"
#include "IniParser.h"
#include "../clientlib/PokerStarsConfig.h"

#include "gtest/gtest.h"

#include <iostream>

#include <boost/filesystem.hpp>

using testing::Range;
using testing::Combine;

Log g_Logger;

void DoTest()
{
    clnt::ps::Config cfg(g_Logger);
    cfg.Read();
    cfg.Write();

    // old settings
    const clnt::IConfig::Settings settings = cfg.Get();

    clnt::IConfig::Settings newSettings;
    newSettings.m_FoldButton = 15;
    newSettings.m_CheckCallButton = 17;
    newSettings.m_RaiseButton = 20;

    cfg.Set(newSettings);

    cfg.Write();

    cfg.Read();

    const clnt::IConfig::Settings& newReadSettings = cfg.Get();
    
    EXPECT_EQ(newSettings.m_FoldButton, newReadSettings.m_FoldButton);
    EXPECT_EQ(newSettings.m_CheckCallButton, newReadSettings.m_CheckCallButton);
    EXPECT_EQ(newSettings.m_RaiseButton, newReadSettings.m_RaiseButton);

    cfg.Set(settings);
    cfg.Write();
}

TEST(Settings, Simple)
{
    DoTest();
}