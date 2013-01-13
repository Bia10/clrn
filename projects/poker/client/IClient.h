#ifndef IClient_h__
#define IClient_h__

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

}

#endif // IClient_h__
