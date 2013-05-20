#include "PokerStarsTableControl.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/random_device.hpp>
#include <boost/shared_ptr.hpp>

namespace clnt
{
namespace ps
{

boost::random::random_device g_Random;

TableControl::TableControl(ILog& logger, boost::asio::io_service& io, IConfig& cfg, HWND window) : m_Log(logger), m_Io(io), m_Window(window)
{
    // read config data
    cfg.Read();
    m_Settings = cfg.Get();
}

void TableControl::Fold()
{
    const TimerPtr timer(new boost::asio::deadline_timer(m_Io));

    boost::random::uniform_int_distribution<> wait(1000, 3000);
    timer->expires_from_now(boost::posix_time::milliseconds(wait(g_Random)));
    timer->async_wait(boost::bind(&TableControl::TimerCallback, this, _1, m_Window, m_Settings.m_FoldButton, timer));
}

void TableControl::CheckCall()
{
    const TimerPtr timer(new boost::asio::deadline_timer(m_Io));

    boost::random::uniform_int_distribution<> wait(1000, 2000);
    timer->expires_from_now(boost::posix_time::milliseconds(wait(g_Random)));
    timer->async_wait(boost::bind(&TableControl::TimerCallback, this, _1, m_Window, m_Settings.m_CheckCallButton, timer));
}


void TableControl::BetRaise()
{
    const TimerPtr timer(new boost::asio::deadline_timer(m_Io));

    boost::random::uniform_int_distribution<> wait(2000, 4000);
    timer->expires_from_now(boost::posix_time::milliseconds(wait(g_Random)));
    timer->async_wait(boost::bind(&TableControl::TimerCallback, this, _1, m_Window, m_Settings.m_RaiseButton, timer));
}

void TableControl::TimerCallback(const boost::system::error_code& e, HWND window, unsigned key, TimerPtr)
{
    const HWND lastWindow = GetForegroundWindow();
    ShowWindow(window, SW_HIDE);
    SetForegroundWindow(window);
    ShowWindow(window, SW_SHOWMINIMIZED);

    KEYBDINPUT kbinput;
    ZeroMemory(&kbinput, sizeof(kbinput));
    kbinput.wVk = key;
    kbinput.dwFlags = 0; 
    kbinput.time = 0;

    INPUT input;
    ZeroMemory(&input, sizeof(input));
    input.type = INPUT_KEYBOARD;
    input.ki = kbinput;

    SendInput(1, &input, sizeof(input));
    ShowWindow(lastWindow, SW_SHOWNORMAL);
    SetForegroundWindow(lastWindow);
}

} // namespace ps
} // namespace clnt