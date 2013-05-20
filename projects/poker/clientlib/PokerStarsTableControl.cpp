#include "PokerStarsTableControl.h"
#include "Modules.h"

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/random_device.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace clnt
{
namespace ps
{

boost::random::random_device g_Random;

const int CURRENT_MODULE_ID = Modules::Client;

TableControl::TableControl(ILog& logger, boost::asio::io_service& io, IConfig& cfg, HWND window) : m_Log(logger), m_Io(io), m_Window(window)
{
    SCOPED_LOG(m_Log);

    // read config data
    cfg.Read();
    m_Settings = cfg.Get();
}

void TableControl::Fold()
{
    SCOPED_LOG(m_Log);

    const TimerPtr timer(new boost::asio::deadline_timer(m_Io));

    boost::random::uniform_int_distribution<> wait(1000, 3000);
    timer->expires_from_now(boost::posix_time::milliseconds(wait(g_Random)));
    timer->async_wait(boost::bind(&TableControl::TimerCallback, this, _1, m_Window, m_Settings.m_FoldButton, timer, boost::function<void()>()));
}

void TableControl::CheckCall()
{
    SCOPED_LOG(m_Log);

    const TimerPtr timer(new boost::asio::deadline_timer(m_Io));

    boost::random::uniform_int_distribution<> wait(1000, 2000);
    timer->expires_from_now(boost::posix_time::milliseconds(wait(g_Random)));
    timer->async_wait(boost::bind(&TableControl::TimerCallback, this, _1, m_Window, m_Settings.m_CheckCallButton, timer, boost::function<void()>()));
}


void TableControl::BetRaise(unsigned amount)
{
    SCOPED_LOG(m_Log);

    const TimerPtr timer(new boost::asio::deadline_timer(m_Io));

    boost::random::uniform_int_distribution<> wait(2000, 4000);
    timer->expires_from_now(boost::posix_time::milliseconds(wait(g_Random)));
    timer->async_wait
    (
        boost::bind
        (
            &TableControl::TimerCallback,
            this, 
            _1, 
            m_Window, 
            m_Settings.m_RaiseButton, 
            timer, 
            boost::function<void()>
            (
                boost::bind
                (
                    &TableControl::SetBetAmount, 
                    this, 
                    amount
                )
            )
         )
     );
}

void TableControl::TimerCallback(const boost::system::error_code& e, HWND window, unsigned key, TimerPtr, const boost::function<void()>& work)
{
    SCOPED_LOG(m_Log);

    TRY 
    {
	    const HWND lastWindow = GetForegroundWindow();
        if (lastWindow != window)
        {
            ShowWindow(window, SW_HIDE);
            SetForegroundWindow(window);
            ShowWindow(window, SW_SHOWMINIMIZED);
        }
	
	    if (work)
	        work();
	
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

        if (lastWindow != window)
        {
            ShowWindow(lastWindow, SW_SHOWNORMAL);
            SetForegroundWindow(lastWindow);
        }
    }
    CATCH_IGNORE_EXCEPTIONS(m_Log, "Timer callback failed", window, key)
}

void TableControl::SetBetAmount(unsigned amount)
{
    SCOPED_LOG(m_Log);

    const HWND slider = FindWindowEx(m_Window, NULL, "PokerStarsSliderClass", NULL);	
    CHECK(slider != NULL, "Failed to find slider");
    const HWND editor = FindWindowEx(slider, NULL, "PokerStarsSliderEditorClass", NULL);
    CHECK(editor != NULL, "Failed to find editor");

    const std::string value = boost::lexical_cast<std::string>(amount);
    SendMessage(editor, WM_SETTEXT, NULL, reinterpret_cast<LPARAM>(value.c_str()));
}

} // namespace ps
} // namespace clnt