// DllInjector.cpp : Defines the entry point for the console application.
//

#include "Injector.h"

#include <iostream>

int main(int argc, char* argv[])
{
	try 
	{
		if (argc < 3)
			return 1;

		dll::Injector injector(argv[1]);

		injector.Inject(argv[2]);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}

