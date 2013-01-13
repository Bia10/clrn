// dllmain.cpp : Defines the entry point for the DLL application.
#include "Detours.h"
#include "PokerStars.h"

#include <vector>
#include <assert.h>
#include <string>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <windows.h>
#include <stdlib.h>

#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/thread/mutex.hpp>

using namespace dasm;

typedef void (__cdecl* ParseMessageOnTheFlyOriginal)(int, MessageRecord *);
typedef BOOL (WINAPI* SendPostMessage)(HWND , UINT,  WPARAM, LPARAM);
typedef std::vector<HWND> Windows;
HWND g_Window;

ParseMessageOnTheFlyOriginal g_OriginalParseOnTheFly;
SendPostMessage g_OriginalSendMessageW;
SendPostMessage g_OriginalPostMessageW;
SendPostMessage g_OriginalSendMessageA;
SendPostMessage g_OriginalPostMessageA;
void* g_OriginalHandleMessages;
void* g_OriginalParseMessage;
int g_CardsCount = 0;

class ScopedLogger
{
public:
	ScopedLogger()
	{
		WriteDateTime();
	}

	template<typename T>
	ScopedLogger& operator << (const T& arg)
	{
		m_Stream << arg;
		return *this;
	}

	~ScopedLogger()
	{
		boost::mutex::scoped_lock lock(s_Mutex);
		std::ofstream stream("stat.txt", std::ios_base::app);
		stream << m_Stream.str() << std::endl;
	}

	void WriteDateTime()
	{
		m_Stream << "[" << boost::posix_time::to_iso_extended_string(boost::posix_time::microsec_clock::local_time()) << "] ";
	}

private:
	static boost::mutex s_Mutex;
	std::ostringstream m_Stream;
};

boost::mutex ScopedLogger::s_Mutex;

BOOL CALLBACK PokerWindowFinder(HWND hWnd, LPARAM lParam)
{
	Windows* wnds = reinterpret_cast<Windows*>(lParam);
	wchar_t name[512] = {0};
	BOOL result = GetClassNameW(hWnd, name, _countof(name));
	if (!_wcsicmp(name, L"PokerStarsTableFrameClass"))
		wnds->push_back(hWnd);

	return TRUE;
}

void GetUsefullData(const DataBlock& data, std::vector<char>& out)
{
	std::copy(data.m_Data + data.m_Offset, data.m_Data + data.m_Offset + data.m_Size, std::back_inserter(out));
}

template<typename Stream>
void WriteCardInfo(Stream& stream, const char* data, int index)
{
	stream	<< data[index] << static_cast<int>(data[index + 1]);
}

template<typename Stream>
void BytesToString(Stream& stream, const void* data, std::size_t size)
{
	for (std::size_t i = 0 ; i < size; ++i)
	{
		const unsigned char symbol = reinterpret_cast<const unsigned char*>(data)[i];
		stream 
			<< std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(symbol)
			<< std::setw(0) << "'" << std::dec;

		if (symbol < 0x20)
			stream << " ";
		else
			stream << symbol;
		stream << "' ";
	}
}

bool IsCard(const char* data)
{
	if (data[0] != 'd' && data[0] != 's' && data[0] != 'h' && data[0] != 'c')
		return false;

	if (data[1] < 0x02 || data[1] > 0x0e)
		return false;
	return true;
}

bool IsCardsEnd(const char* data)
{
	return data[0] == 0x75 && data[1] == 0x00;
}

void FindFlopCards(const DataBlock& data, std::string& result)
{
	int cardsFound = 0;
	for (unsigned int i = data.m_Offset ; i < data.m_Size + data.m_Offset; ++i)
	{
		const char* current = data.m_Data + i;
		if (IsCard(current))
		{
			++cardsFound;
			++i;
			std::ostringstream oss;
			WriteCardInfo(oss, current, 0);
			result += oss.str();
		}
		else
		if (cardsFound)
		{
			if (cardsFound >= 3 && IsCardsEnd(current))
				return;

			cardsFound = 0;
			result.clear();
		}
	}
}

void MessagesHook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (hWnd != g_Window || Msg != 0x8002)
		return;

	WindowMessage* message = reinterpret_cast<WindowMessage*>(lParam);

	std::vector<char> data;
	GetUsefullData(message->m_Block, data);

	const DWORD messageId = _byteswap_ulong(*reinterpret_cast<const DWORD*>(&data[0xd]));

	ScopedLogger logger;
	logger << "Message Id: 0x" << std::hex <<  std::setfill('0') << std::setw(4) << messageId;

	if (messageId == 0x10000)
	{
		const char* name = &data[0x31];

		const std::string playerName(name);
		logger << ", Player name: " << playerName;

		const char action = data[0x2c];
		const int amount = _byteswap_ulong(*reinterpret_cast<const int*>(&data[0x2d]));	

		logger 
			<< ", Action: 0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(action) << " '" << action 
			<< "', Amount: " << std::setw(0) << std::dec << amount / 100;

		if (action == 'C')
		{
			const int stack = _byteswap_ulong(*reinterpret_cast<const int*>(&data[0x88 + playerName.size()]));	
			logger << ", Stack: " << stack  / 100;
		}
	}
	else
	if (messageId == 0x1001 || messageId == 0x1005) // flop cards
	{
		std::string cards;
		FindFlopCards(message->m_Block, cards);
		logger << ", Flop cards: " << cards;
	}
	else
	if (messageId == 0x1800) // player cards
	{
		std::ostringstream oss;

		WriteCardInfo(oss, &data.front(), 0x15);
		WriteCardInfo(oss, &data.front(), 0x18);

		logger << ", Player cards: " << oss.str();
	}
	else
	if (messageId == 0x20000) // players info
	{
		MessageRecord record(*message);
		record.SetParsedBytes(0x20);

		std::ostringstream oss;

		for (;;)
		{
			std::string name;
			record.Extract(name);

			if (name.empty())
				break;

			DWORD stack;
			record.Extract(stack);
			stack = _byteswap_ulong(stack);

			std::string country;
			record.Extract(country);

			DWORD temp;
			record.Extract(temp);
			record.Extract(temp);

			oss << ", Player: " << name << ", Stack: " << stack / 100 << ", Country: " << country;
		}

		logger << oss.str();
	}

	logger << ", Data: ";
	BytesToString(logger, &data.front(), data.size());
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
	Windows windows;
	assert(EnumWindows(&PokerWindowFinder, reinterpret_cast<LPARAM>(&windows)));
	assert(!windows.empty());
	g_Window = windows.front();


/*
	Detours::HookFunction(reinterpret_cast<void*>(0x008AFB20), &ParseMessageOnTheFly, reinterpret_cast<void**>(&g_OriginalParseOnTheFly), 7); 
	Detours::HookFunction(reinterpret_cast<void*>(0x006DB020), &HandleMessagesWrapper, reinterpret_cast<void**>(&g_OriginalHandleMessages), 8);
	Detours::HookFunction(reinterpret_cast<void*>(0x00415210), &ParseMessagesWrapper, reinterpret_cast<void**>(&g_OriginalParseMessage), 7);*/

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

