#include "PokerStarsClient.h"
#include "Exception.h"
#include "PokerStarsTable.h"
#include "Modules.h"

#include <windows.h>

namespace clnt
{
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
	BOOL result = GetClassNameW(hWnd, name, _countof(name));
	if (!_wcsicmp(name, L"PokerStarsTableFrameClass"))
		wnds.m_Matched.push_back(hWnd);
	else
		wnds.m_NotMatched.push_back(hWnd);

	return TRUE;
}

void Client::HandleMessage(HWND hWnd, UINT Msg, WPARAM /*wParam*/, LPARAM lParam)
{
	if (Msg != 0x8002)
		return;

	if (std::find(m_NotInterested.begin(), m_NotInterested.end(), hWnd) != m_NotInterested.end())
		return;

	boost::mutex::scoped_lock lock(m_Mutex);
	Tables::const_iterator it = m_Tables.find(hWnd);
	if (it == m_Tables.end())
		it = m_Tables.insert(std::make_pair(hWnd, ITable::Ptr(new ps::Table(m_Log)))).first;

	lock.unlock();

	const dasm::WindowMessage& message = *reinterpret_cast<const dasm::WindowMessage*>(lParam);

	it->second->HandleMessage(message);
}

Client::Client()
{
	m_Log.Open("client.txt", Modules::Client, ILog::Level::Debug);
	m_Log.Open("mesages.txt", Modules::Messages, ILog::Level::Debug);
	m_Log.Open("table.txt", Modules::Table, ILog::Level::Debug);

	// find initial windows
	Windows wnds;

	CHECK(EnumWindows(&PokerWindowFinder, reinterpret_cast<LPARAM>(&wnds)));

	m_NotInterested = wnds.m_NotMatched;

	for (const auto handle : wnds.m_Matched)
		m_Tables.insert(std::make_pair(handle, ITable::Ptr(new ps::Table(m_Log))));
}

} // namespace ps
} // namespace clnt