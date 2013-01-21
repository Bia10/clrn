#ifndef PokerStarsClient_h__
#define PokerStarsClient_h__

#include "IClient.h"
#include "ITable.h"
#include "Log.h"
#include "IDataSender.h"

#include <map>
#include <vector>

#include <boost/thread/mutex.hpp>

namespace pcmn
{
	class Screenshot;
}

namespace clnt
{
namespace ps
{
//! Poker client
class Client : public IClient
{
	typedef std::map<HWND, ITable::Ptr> Tables;
	typedef std::map<HWND, std::wstring> WindowClasses;
public:

	Client();

private:

	virtual void HandleMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) override;

	void SaveScreenThread(HWND handle, const std::string& message);
	void TakeScreenshot(pcmn::Screenshot& screenshot, unsigned index, const std::wstring& path);
	const std::wstring& GetWindowClass(HWND handle);

private:

	//! Tables
	Tables m_Tables;

	//! Window classes
	WindowClasses m_WindowClasses;

	//! Tables mutex
	boost::mutex m_Mutex;

	//! Logger
	Log m_Log;

	//! Data sender
	std::auto_ptr<pcmn::IDataSender> m_Sender;
};

} // namespace ps
} // namespace clnt

#endif // PokerStarsClient_h__
