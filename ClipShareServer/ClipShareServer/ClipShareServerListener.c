#include <WS2tcpip.h>
#include <WinSock2.h>
#include <windows.h>

#include "Logger.h"

#define	SERVER_LISTENER_PORT	7489

HANDLE hListenerThread;

DWORD WINAPI ServerListenerThread(LPVOID lpParam)
{
	SOCKET sockServer;
	WSADATA wsaData;
	struct sockaddr_in saServer;

	if(WSAStartup(MAKEWORD(2,2), &wsaData))
	{
		LogMessage("WSAStartup failed.");
		return 0;
	}

	saServer.sin_family = AF_INET;
	saServer.sin_port = htons((u_short)SERVER_LISTENER_PORT);
	saServer.sin_addr.s_addr = INADDR_ANY;

	sockServer = socket(AF_INET, SOCK_STREAM, 0);
	if(sockServer == INVALID_SOCKET)
	{
		LogMessage("Unable to create server socket.");
		return 0;
	}

	if(bind(sockServer, (struct sockaddr *)&saServer, sizeof(saServer)))
	{
		LogMessage("Unable to bind server socket to port.");
		return 0;
	}

	if(listen(sockServer, 5))
	{
		LogMessage("Unable to set server socket in listening mode.");
		return 0;
	}

	closesocket(sockServer);
	WSACleanup();

	return 1;
}

DWORD InitServerListener()
{
	hListenerThread = CreateThread(NULL, 0, ServerListenerThread, NULL, 0, NULL);

	if(!hListenerThread)
		return 0;

	return 1;
}