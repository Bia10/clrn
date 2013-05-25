// SocketAsyncTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include <iostream>

#include "Connection.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>

class CTest
{
public:

	void AcceptHandler(const CConnection::CIOInfo& Info)
	{
		Info.eEvent;
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		CTest Test;

		CConnection Connection;
		Connection.RegisterCallback(CConnection::CEvent::eConnect, boost::bind(&CTest::AcceptHandler, &Test, _1));
		Connection.Run(5);
		Connection.Create(27100);
		while(true)
			Sleep(100);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}


	
	return 0;
}

