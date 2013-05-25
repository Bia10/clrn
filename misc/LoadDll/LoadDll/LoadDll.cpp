// LoadDll.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "resource.h"
#include "Loader.h"

#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/lexical_cast.hpp>

#include <fstream>


boost::mutex Mutex;
boost::mutex Mutex1;
typedef int (WINAPI *pFunction)();

void Test(LPVOID LpLib, int nThreadID)
{
	HMODULE hLib;
	pFunction Start;
	std::string sFileName("C:\\log");
	std::string sTemp = boost::lexical_cast< std::string > (nThreadID);
	sFileName.append(sTemp);
	sFileName.append(".txt");
	std::ofstream fs(sFileName.c_str(), std::ios_base::app);
	{
		boost::mutex::scoped_lock lock(Mutex);
		hLib = MemLoadLibrary((PBYTE)LpLib);
	    Start = (pFunction) MemGetProcAddress(hLib, "fnDll");
	}
	
	for(int i = 0; i < 200; ++i)
	{
		int nResult = Start();
		fs << "Thread id: " << nThreadID << " result: " << nResult << std::endl;
		{
			//boost::mutex::scoped_lock lock(Mutex);
			//std::cout << "Thread id: " << nThreadID << " result: " << nResult << std::endl;
		}
	}
	{
		boost::mutex::scoped_lock lock(Mutex1);
		MemFreeLibrary(hLib);
	}
}


int _tmain(int argc, _TCHAR* argv[])
{

	HMODULE hExe = ::GetModuleHandle(NULL);

    HRSRC hRes;
    HGLOBAL hResLoad;
    DWORD dwResSize;

	hRes = ::FindResourceW(hExe, MAKEINTRESOURCE(IDR_DLL2), L"DLL");
	dwResSize = ::SizeofResource(hExe, hRes);
	hResLoad = ::LoadResource(hExe, hRes);
	LPVOID lpResData = ::LockResource(hResLoad);

	boost::thread t1(boost::bind(Test, lpResData, 1));
	boost::thread t2(boost::bind(Test, lpResData, 2));
	boost::thread t3(boost::bind(Test, lpResData, 3));/*
	boost::thread t4(boost::bind(Test, lpResData, 4));
	boost::thread t5(boost::bind(Test, lpResData, 5));*/

	t1.join();
	t2.join();
	t3.join();/*
	t4.join();
	t5.join();
*/
	return 0;
}

