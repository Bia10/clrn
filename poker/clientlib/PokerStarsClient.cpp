#include "PokerStarsClient.h"
#include "Exception.h"
#include "PokerStarsTable.h"
#include "Modules.h"
#include "Screenshot.h"
#include "FileSystem.h"
#include "UDPHost.h"
#include "Player.h"
#include "Config.h"
#include "PokerStarsTableControl.h"
#include "PokerStarsConfig.h"

#include <windows.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

namespace clnt
{

const wchar_t POKERSTARS_TABLE_WINDOW_CLASS[] = L"PokerStarsTableFrameClass";
const int CURRENT_MODULE_ID = Modules::Client;

IClient* CreateClient()
{
	return new ps::Client();
}

namespace ps
{

void Client::HandleMessage(HWND hWnd, UINT Msg, WPARAM /*wParam*/, LPARAM lParam)
{
	SCOPED_LOG(m_Log);

	if (Msg != 0x8002)
		return;

	const std::wstring& name = GetWindowClass(hWnd);

	if (name != POKERSTARS_TABLE_WINDOW_CLASS)
		return; // not a poker window

	try 
	{
		boost::mutex::scoped_lock lock(m_Mutex);
		Tables::const_iterator it = m_Tables.find(hWnd);
		if (it == m_Tables.end())
		{
            const static pcmn::Evaluator evaluator;
			const net::IConnection::Ptr connection = m_Server->Connect("127.0.0.1", cfg::DEFAULT_PORT);
            std::auto_ptr<ITableControl> ctrl(new TableControl(m_Log, m_Server->GetService(), *m_Cfg, hWnd));
			it = m_Tables.insert(std::make_pair(hWnd, ITable::Ptr(new ps::Table(m_Log, hWnd, connection, evaluator, ctrl.get())))).first;
            ctrl.release();
		}
	
		lock.unlock();
	
		const dasm::WindowMessage& message = *reinterpret_cast<const dasm::WindowMessage*>(lParam);
	
		it->second->HandleMessage(message);
	}
	catch (const std::exception& e)
	{
		if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Error))
			boost::thread(boost::bind(&Client::SaveScreenThread, this, hWnd, std::string(e.what())));
	}
	catch (...)
	{
		if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Error))
			boost::thread(boost::bind(&Client::SaveScreenThread, this, hWnd, std::string("Unhandled error")));
	}
}

Client::Client() : m_Server(new net::UDPHost(m_Log, 1)), m_Cfg(new Config(m_Log))
{
	TRY 
	{
		SCOPED_LOG(m_Log);

		m_Log.Open("logs/network.txt", Modules::Network, ILog::Level::Trace);
		m_Log.Open("logs/client.txt", Modules::Client, ILog::Level::Trace);
		m_Log.Open("logs/messages.txt", Modules::Messages, ILog::Level::Trace);
        m_Log.Open("logs/table.txt", Modules::Table, ILog::Level::Debug);
        m_Log.Open("logs/logic.txt", Modules::TableLogic, ILog::Level::Debug);

		// player name
		pcmn::Player::ThisPlayer().Name("clrntest");
	}
	CATCH_PASS_EXCEPTIONS("Failed to construct client")
}

void Client::SaveScreenThread(HWND handle, const std::string& message)
{
	SCOPED_LOG(m_Log);
		
	try 
	{
		std::wstring path = fs::GetInitialPath() + L"/logs/";
		fs::CreateDirectories(path);

		std::wstring time = boost::posix_time::to_iso_extended_wstring(boost::posix_time::microsec_clock().local_time());
		boost::algorithm::replace_all(time, L":", L".");
		boost::algorithm::replace_all(time, L"T", L"_");

		path += time;
		path = fs::FullPath(path);

		LOG_ERROR("Handle message failed: [%s], file: [%s]") % message % path;

		ShowWindow(handle, SW_SHOW);
		pcmn::Screenshot shot(handle);
		for (unsigned i = 0 ; i < 10; ++i)
		{
			TakeScreenshot(shot, i, path);
			boost::this_thread::interruptible_wait(200);
		}
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("Failed to save screenshot, error: [%s]") % e.what();
	}
}

void Client::TakeScreenshot(pcmn::Screenshot& scrrenshot, unsigned index, const std::wstring& path)
{
	const std::wstring temp = path + std::wstring(L"_") + conv::cast<std::wstring>(index) + L".bmp";
	scrrenshot.Save(fs::FullPath(temp));
}

const std::wstring& Client::GetWindowClass(HWND handle)
{
	WindowClasses::const_iterator it = m_WindowClasses.find(handle);
	if (it == m_WindowClasses.end())
	{
		wchar_t name[512] = {0};
		CHECK(GetClassNameW(handle, name, _countof(name)));
		it = m_WindowClasses.insert(std::make_pair(handle, name)).first;
	}

	return it->second;
}


} // namespace ps
} // namespace clnt

