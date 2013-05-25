// SocketServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include <iostream>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27100"

int __cdecl main(void) 
{
	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET,
		ClientSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,  // connection info
		hints;
	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) 
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if ( iResult != 0 ) 
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) 
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);



	// Включаем прослушивание
	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
Listen:
	std::cout << "Waiting for client..." << std::endl;

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	std::cout << "Got client!" << std::endl;

	//goto Listen;

	// No longer need server socket
	//closesocket(ListenSocket);

	// Receive until the peer shuts down the connection

	std::size_t nIterations = 0;
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) 
		{
			printf("Bytes received: %d\n", iResult);

			recvbuf[iResult] = '\0';

			std::cout << "Recieved: " << recvbuf << std::endl;

			// Echo the buffer back to the sender
			iSendResult = send( ClientSocket, recvbuf, iResult, 0 );
			if (iSendResult == SOCKET_ERROR) 
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				break;
				closesocket(ClientSocket);
				WSACleanup();
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else  
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			break;
			closesocket(ClientSocket);
			WSACleanup();
		}
		++nIterations;
	} 
	while (iResult > 0/* && nIterations < 15*/);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	goto Listen;

	WSACleanup();

	return 0;
}

