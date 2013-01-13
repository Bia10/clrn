// DllInjector.cpp : Defines the entry point for the console application.
//

#include "Injector.h"

#include <iostream>

int main(int argc, char* argv[])
{
	try 
	{
		dll::Injector injector("PokerStars.exe");
		injector.Inject("d:\\svn\\trunk\\projects\\poker\\tests\\SpyDll\\Debug\\SpyDll.dll");
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}

