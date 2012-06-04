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

		kernel.Init(argc == 2 ? argv[1] : 0);
		kernel.Run();
	}
	catch (const std::exception& e)
	{	
		std::cout << "Error: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unhandled error." << std::endl;
	}

	return 0;
}

