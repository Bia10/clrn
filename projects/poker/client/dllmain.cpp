// dllmain.cpp : Defines the entry point for the DLL application.
#include "Detours.h"
#include <windows.h>
#include <cassert>

typedef BOOL (WINAPI* SendPostMessage)(HWND , UINT,  WPARAM, LPARAM);

SendPostMessage g_OriginalSendMessageW;
SendPostMessage g_OriginalPostMessageW;
SendPostMessage g_OriginalSendMessageA;
SendPostMessage g_OriginalPostMessageA;

void MessagesHook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{

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

void Intercept()
{
	HMODULE user32 = GetModuleHandleW(L"User32.dll");
	assert(user32);

	// window messages
	Detours::HookFunction(reinterpret_cast<void*>(GetProcAddress(user32, "SendMessageW")), &SendMessageWHook, reinterpret_cast<void**>(&g_OriginalSendMessageW));
	Detours::HookFunction(reinterpret_cast<void*>(GetProcAddress(user32, "SendMessageA")), &SendMessageAHook, reinterpret_cast<void**>(&g_OriginalSendMessageA));
	Detours::HookFunction(reinterpret_cast<void*>(GetProcAddress(user32, "PostMessageW")), &PostMessageWHook, reinterpret_cast<void**>(&g_OriginalPostMessageW));
	Detours::HookFunction(reinterpret_cast<void*>(GetProcAddress(user32, "PostMessageA")), &PostMessageAHook, reinterpret_cast<void**>(&g_OriginalPostMessageA));
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
		Intercept();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

