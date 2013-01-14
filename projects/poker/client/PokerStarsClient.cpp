#include "PokerStarsClient.h"
#include "Exception.h"
#include "PokerStarsTable.h"
#include "Modules.h"
#include "Screenshot.h"
#include "FileSystem.h"

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

struct Windows
{
	typedef std::vector<HWND> List;

	List m_Matched;
	List m_NotMatched;
};

BOOL CALLBACK PokerWindowFinder(HWND hWnd, LPARAM lParam)
{
	Windows& wnds = *reinterpret_cast<Windows*>(lParam);
	wchar_t name[512] = {0};
	CHECK(GetClassNameW(hWnd, name, _countof(name)));
	if (!_wcsicmp(name, POKERSTARS_TABLE_WINDOW_CLASS))
		wnds.m_Matched.push_back(hWnd);
	else
		wnds.m_NotMatched.push_back(hWnd);

	return TRUE;
}

void Client::HandleMessage(HWND hWnd, UINT Msg, WPARAM /*wParam*/, LPARAM lParam)
{
	SCOPED_LOG(m_Log);

	if (Msg != 0x8002)
		return;

	if (std::find(m_NotInterested.begin(), m_NotInterested.end(), hWnd) != m_NotInterested.end())
		return;

	wchar_t name[512] = {0};
	CHECK(GetClassNameW(hWnd, name, _countof(name)));

	if (_wcsicmp(name, POKERSTARS_TABLE_WINDOW_CLASS))
		return; // not a poker window

	try 
	{
		boost::mutex::scoped_lock lock(m_Mutex);
		Tables::const_iterator it = m_Tables.find(hWnd);
		if (it == m_Tables.end())
			it = m_Tables.insert(std::make_pair(hWnd, ITable::Ptr(new ps::Table(m_Log)))).first;
	
		lock.unlock();
	
		const dasm::WindowMessage& message = *reinterpret_cast<const dasm::WindowMessage*>(lParam);
	
		it->second->HandleMessage(message);
	}
	catch (const std::exception& e)
	{
		if (m_Log.IsEnabled(CURRENT_MODULE_ID, ILog::Level::Error))
			boost::thread(boost::bind(&Client::SaveScreenThread, this, hWnd, std::string(e.what())));
	}
}

Client::Client()
{
	TRY 
	{
		SCOPED_LOG(m_Log);

		m_Log.Open("logs/client.txt", Modules::Client, ILog::Level::Trace);
		m_Log.Open("logs/mesages.txt", Modules::Messages, ILog::Level::Trace);
		m_Log.Open("logs/table.txt", Modules::Table, ILog::Level::Trace);
	
		// find initial windows
		Windows wnds;
	
		CHECK(EnumWindows(&PokerWindowFinder, reinterpret_cast<LPARAM>(&wnds)));
	
		m_NotInterested = wnds.m_NotMatched;
	
		for (const auto handle : wnds.m_Matched)
			m_Tables.insert(std::make_pair(handle, ITable::Ptr(new ps::Table(m_Log))));
	}
	CATCH_PASS_EXCEPTIONS("Failed to construct client")
}

void Client::SaveScreenThread(HWND handle, const std::string message)
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

		{
			const std::wstring temp = path + L"_1.bmp";
			ShowWindow(handle, SW_SHOW);
			cmn::Screenshot shot(handle);
			shot.Save(fs::FullPath(temp));
		}

		boost::this_thread::interruptible_wait(200);

		{
			const std::wstring temp = path + L"_2.bmp";
			ShowWindow(handle, SW_SHOW);
			cmn::Screenshot shot(handle);
			shot.Save(fs::FullPath(temp));
		}

		boost::this_thread::interruptible_wait(300);

		{
			const std::wstring temp = path + L"_3.bmp";
			ShowWindow(handle, SW_SHOW);
			cmn::Screenshot shot(handle);
			shot.Save(fs::FullPath(temp));
		}

		boost::this_thread::interruptible_wait(500);

		{
			const std::wstring temp = path + L"_4.bmp";
			ShowWindow(handle, SW_SHOW);
			cmn::Screenshot shot(handle);
			shot.Save(fs::FullPath(temp));
		}
	}
	catch (const std::exception& e)
	{
		LOG_ERROR("Failed to save screenshot, error: [%s]") % e.what();
	}
}


} // namespace ps
} // namespace clnt

