#ifndef PokerStarsClient_h__
#define PokerStarsClient_h__

#include "IClient.h"
#include "ITable.h"
#include "Log.h"

#include <map>
#include <vector>

#include <boost/thread/mutex.hpp>

namespace clnt
{
namespace ps
{
//! Poker client
class Client : public IClient
{
	typedef std::map<HWND, ITable::Ptr> Tables;
	typedef std::vector<HWND> WindowList;
public:

	Client();

private:

	virtual void HandleMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) override;

	void SaveScreenThread(HWND handle, const std::string message);

private:

	//! Tables
	Tables m_Tables;

	//! Not interested windows
	WindowList m_NotInterested;

	//! Tables mutex
	boost::mutex m_Mutex;

	//! Logger
	Log m_Log;
};

} // namespace ps
} // namespace clnt

#endif // PokerStarsClient_h__
