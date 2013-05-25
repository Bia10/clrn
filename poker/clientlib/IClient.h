#ifndef IClient_h__
#define IClient_h__

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <boost/noncopyable.hpp>

namespace clnt
{

//! Poker client abstraction
class IClient : boost::noncopyable
{
public:
	virtual ~IClient() {}

	virtual void HandleMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) = 0;
};

IClient* CreateClient();

}

#endif // IClient_h__
