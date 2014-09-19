#include <WS2tcpip.h>
#include <WinSock2.h>
#include <windows.h>

#define	SERVER_LISTENER_PORT	7489

HANDLE hListenerThread;

DWORD WINAPI ServerListenerThread(LPVOID lpParam)
{
	SOCKET sockServer;
	WSADATA wsaData;
	struct sockaddr_in saServer;

	if(WSAStartup(MAKEWORD(2,2), &wsaData))
	{
		//Do something
		return -1;
	}

	saServer.sin_family = AF_INET;
	saServer.sin_port = htons((u_short)SERVER_LISTENER_PORT);
	saServer.sin_addr.s_addr = INADDR_ANY;

	sockServer = socket(AF_INET, SOCK_STREAM, 0);
	if(sockServer == INVALID_SOCKET)
	{
		//Do something
		return -1;
	}

	if(bind(sockServer, (struct sockaddr *)&saServer, sizeof(saServer)))
	{
		//Do something
		return -1;
	}

	if(listen(sockServer, 5))
	{
		//Do something
		return -1;
	}

	closesocket(sockServer);
	WSACleanup();

	return 0;
}

DWORD InitServerListener()
{
	hListenerThread = CreateThread(NULL, 0, ServerListenerThread, NULL, 0, NULL);

	if(!hListenerThread)
	{
		//Do something
		return 0;
	}
}