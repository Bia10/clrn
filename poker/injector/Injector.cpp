#include "Injector.h"
#include <windows.h>
#include <TlHelp32.h>
#include <sstream>
#include <assert.h>

namespace dll
{

//структура описывает пол€, в которых содержитс€ код внедрени€
struct InjectCode
{
	BYTE  instr_push_loadlibrary_arg; //инструкци€ push
	DWORD loadlibrary_arg;            //аргумент push  

	WORD  instr_call_loadlibrary;     //инструкци€ call []  
	DWORD adr_from_call_loadlibrary;

	BYTE  instr_push_exitthread_arg;
	DWORD exitthread_arg;

	WORD  instr_call_exitthread;
	DWORD adr_from_call_exitthread;

	DWORD addr_loadlibrary;
	DWORD addr_exitthread;     //адрес функции ExitTHread
	char  libraryname[100];    //им€ и путь к загружаемой библиотеке  
};

Injector::Injector(std::size_t id) : m_ProcessId(id)
{
}

Injector::Injector(const std::string& name) : m_ProcessId(0)
{
	HANDLE          hSnap;
	PROCESSENTRY32  peProc;

	if((hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) != INVALID_HANDLE_VALUE)
	{
		peProc.dwSize = sizeof(PROCESSENTRY32);
		if(Process32First(hSnap, &peProc))
			while(Process32Next(hSnap, &peProc))
				if(!lstrcmpA(name.c_str(), peProc.szExeFile))
				{
					m_ProcessId = peProc.th32ProcessID;
					break;
				}
	}
	assert(m_ProcessId);
	CloseHandle(hSnap);
}


Injector::~Injector()
{
}


void Injector::Inject(const std::string& path)
{
	DWORD   dwMemSize;
	HANDLE  hProc;
	LPVOID  lpRemoteMem, lpLoadLibrary;

	if ((hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_ProcessId)) == NULL)
	{
		std::ostringstream oss;
		oss << "Failed to open process, last error: " << GetLastError();
		throw std::runtime_error(oss.str().c_str());
	}
	
	dwMemSize = strlen(path.c_str());
	if ((lpRemoteMem = VirtualAllocEx(hProc, NULL, dwMemSize, MEM_COMMIT, PAGE_READWRITE)) == NULL)
	{
		std::ostringstream oss;
		oss << "Failed to VirtualAllocEx, last error: " << GetLastError();
		throw std::runtime_error(oss.str().c_str());
	}

	if (WriteProcessMemory(hProc, lpRemoteMem, path.c_str(), dwMemSize, NULL))
	{
		lpLoadLibrary = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
		if (CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)lpLoadLibrary, lpRemoteMem, 0, NULL) == NULL)
		{
			std::ostringstream oss;
			oss << "Failed to create thread, last error: " << GetLastError();
			throw std::runtime_error(oss.str().c_str());
		}
	}
	
	CloseHandle(hProc);

	
// 
// 	if (path.size() > 100)
// 		throw std::invalid_argument("Path too long");
// 
// 	// открыть процесс с нужным доступом
// 	HANDLE hToken; 
// 	TOKEN_PRIVILEGES tp; 
// 	const HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, m_ProcessId); 
// 
// 	tp.PrivilegeCount = 1; 
// 	assert(LookupPrivilegeValueA( NULL, "SeDebugPrivilege", &tp.Privileges[0].Luid )); 
// 	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
// 	assert(OpenProcessToken(process, TOKEN_ADJUST_PRIVILEGES, &hToken)); 
// 
// 	assert(AdjustTokenPrivileges(hToken, FALSE, &tp, NULL, NULL, NULL)); 
// 	CloseHandle(hToken);
// 
// 	if(!process)
// 		throw std::runtime_error("You have not enough rights to attach dlls");
//   
// 	//зарезервировать пам€ть в процессе
// 	BYTE* processMemory = reinterpret_cast<BYTE*>(VirtualAllocEx
// 	(
// 		process, 
// 		0, 
// 		sizeof(InjectCode),
// 		MEM_COMMIT, 
// 		PAGE_EXECUTE_READWRITE
// 	));
// 
// 	if (!processMemory)
// 		throw std::runtime_error("Unable to alloc memory in remote process");
// 
// 	//инициализировать  машинный код
// 	InjectCode cmds;
// 	cmds.instr_push_loadlibrary_arg = 0x68; //машинный код инструкции push
// 	cmds.loadlibrary_arg = (DWORD)((BYTE*)processMemory + offsetof(InjectCode, libraryname));
//   
// 	cmds.instr_call_loadlibrary = 0x15ff; //машинный код инструкции call
// 	cmds.adr_from_call_loadlibrary = (DWORD)(processMemory + offsetof(InjectCode, addr_loadlibrary));
//   
// 	cmds.instr_push_exitthread_arg  = 0x68;
// 	cmds.exitthread_arg = 0;
//   
// 	cmds.instr_call_exitthread = 0x15ff; 
// 	cmds.adr_from_call_exitthread = (DWORD)(processMemory + offsetof(InjectCode, addr_exitthread));
//   
// 	cmds.addr_loadlibrary = (DWORD)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
// 	cmds.addr_exitthread  = (DWORD)GetProcAddress(GetModuleHandleA("kernel32.dll"),"ExitThread");
//   
// 	strcpy_s(cmds.libraryname, path.c_str());
//   
// 	/*ѕосле инициализации cmds в мнемонике ассемблера выгл€дит следующим
// 	образом:
// 		push  adr_library_name               ;аргумент ф-ции loadlibrary
// 		call dword ptr [loadlibrary_adr]     ; вызвать LoadLibrary 
// 		push exit_thread_arg                 ;аргумент дл€ ExitThread
// 		call dword ptr [exit_thread_adr]     ;вызвать ExitThread     
// 	*/
//   
// 	//записать машинный код по зарезервированному адресу
// 	DWORD wr = 0;
// 	assert(WriteProcessMemory(process, processMemory, &cmds, sizeof(cmds), &wr));
// 	assert(wr == sizeof(cmds));
//   
// 	//выполнить машинный код
// 	DWORD threadId = 0;
// 	HANDLE threadHandle = CreateRemoteThread
// 	(
// 		process, 
// 		NULL, 
// 		0, 
// 		(unsigned long (__stdcall *)(void *))processMemory, 
// 		0, 
// 		0, 
// 		&threadId
// 	);
// 
// 	if (!threadId)
// 	{
// 		std::ostringstream oss;
// 		oss << "Failed to create thread, last error: " << GetLastError();
// 		throw std::runtime_error(oss.str().c_str());
// 	}
// 
// 	//ожидать завершени€ удаленного потока
// 	WaitForSingleObject(threadHandle, INFINITE);
// 
// 	//освободить пам€ть
// 	VirtualFreeEx(process, (void*)processMemory, sizeof(cmds), MEM_RELEASE);
}

} // namespace dll