// dllmain.cpp : Defines the entry point for the DLL application.
#include "Detours.h"
#include "IClient.h"

#include <windows.h>
#include <cassert>
#include <memory>

typedef BOOL (WINAPI* SendPostMessage)(HWND , UINT,  WPARAM, LPARAM);

SendPostMessage g_OriginalSendMessageW;
SendPostMessage g_OriginalPostMessageW;
SendPostMessage g_OriginalSendMessageA;
SendPostMessage g_OriginalPostMessageA;
std::auto_ptr<clnt::IClient> g_Client;
HMODULE g_Module;

void MessagesHook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	try
	{
		g_Client->HandleMessage(hWnd, Msg, wParam, lParam);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(0, e.what(), "Error", MB_ICONERROR);
	}
	catch (...)
	{
		MessageBoxA(0, "Unhandled error", "Error", MB_ICONERROR);		
		FreeLibrary(g_Module);
	}
}

BOOL WINAPI SendMessageWHook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	MessagesHook(hWnd, Msg, wParam, lParam);
	return g_OriginalSendMessageW(hWnd, Msg, wParam, lParam);
}
BOOL WINAPI SendMessageAHook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	MessagesHook(hWnd, Msg, wParam, lParam);
	return g_OriginalSendMessageA(hWnd, Msg, wParam, lParam);
}
BOOL WINAPI PostMessageWHook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	MessagesHook(hWnd, Msg, wParam, lParam);
	return g_OriginalPostMessageW(hWnd, Msg, wParam, lParam);
}
BOOL WINAPI PostMessageAHook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	MessagesHook(hWnd, Msg, wParam, lParam);
	return g_OriginalPostMessageA(hWnd, Msg, wParam, lParam);
}

void Init()
{
	try
	{
		// create new client
		g_Client.reset(clnt::CreateClient());

		const HMODULE user32 = GetModuleHandleW(L"User32.dll");
		assert(user32);

		// window messages
		Detours::HookFunction(reinterpret_cast<void*>(GetProcAddress(user32, "SendMessageW")), &SendMessageWHook, reinterpret_cast<void**>(&g_OriginalSendMessageW));
		Detours::HookFunction(reinterpret_cast<void*>(GetProcAddress(user32, "SendMessageA")), &SendMessageAHook, reinterpret_cast<void**>(&g_OriginalSendMessageA));
		Detours::HookFunction(reinterpret_cast<void*>(GetProcAddress(user32, "PostMessageW")), &PostMessageWHook, reinterpret_cast<void**>(&g_OriginalPostMessageW));
		Detours::HookFunction(reinterpret_cast<void*>(GetProcAddress(user32, "PostMessageA")), &PostMessageAHook, reinterpret_cast<void**>(&g_OriginalPostMessageA));
	}
	catch (const std::exception& e)
	{
		MessageBoxA(0, e.what(), "Error", MB_ICONERROR);
		FreeLibrary(g_Module);
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		MessageBoxA(0, "Attached.", "Result", 0);
		Init();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

