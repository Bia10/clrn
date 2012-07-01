// Kernel.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Kernel.h"
#include "FileSystem.h"

int main(int argc, char* argv[])
{ 	
	try 
	{	
		fs::GetInitialPath();	

		CKernel kernel;

		kernel.Init(argc > 1 ? argv[1] : 0);
		kernel.Run();
	}
	catch (const std::exception& e)
	{	
		std::cout << "Kernel init error: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Kernel unhandled error." << std::endl;
	}

	return 0;
}

