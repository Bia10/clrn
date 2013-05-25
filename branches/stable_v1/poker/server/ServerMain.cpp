#include <iostream>
#include "../serverlib/Server.h"

int main(int argc, char* argv[])
{
	try 
	{
		srv::Server srv;
		srv.Run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}

