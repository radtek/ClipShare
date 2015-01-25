#include <WinSock2.h>
#include <WS2tcpip.h>
#include <sstream>

#include "ClipShareServerListener.h"

ClipShareServerListener *ClipShareServerListener::pServerListener = NULL;

ClipShareServerListener::ClipShareServerListener(Logger *_logger)
{
	logger = *_logger;
	
	hServiceStopEvt = NULL;
	hConnectionEndEvt = NULL;
	bServiceStopping = false;
}

void ClipShareServerListener::SetServiceStopEvt()
{
	SetEvent(hServiceStopEvt);
}

DWORD ClipShareServerListener::InitServerListenerWorker()
{
	WSADATA wsaData;
	struct sockaddr_in saServer;

	if(WSAStartup(MAKEWORD(2,2), &wsaData))
	{
		logger.LogMessage("WSAStartup failed.");
		return 0;
	}

	saServer.sin_family = AF_INET;
	saServer.sin_port = htons((u_short)SERVER_LISTENER_PORT);
	saServer.sin_addr.s_addr = INADDR_ANY;

	sockServer = socket(AF_INET, SOCK_STREAM, 0);
	if(sockServer == INVALID_SOCKET)
	{
		logger.LogMessage("Unable to create server socket.");
		return 0;
	}

	if(bind(sockServer, (struct sockaddr *)&saServer, sizeof(saServer)))
	{
		logger.LogMessage("Unable to bind server socket to port.");
		return 0;
	}

	if(listen(sockServer, 5))
	{
		logger.LogMessage("Unable to set server socket in listening mode.");
		return 0;
	}

	hConnectionEndEvt = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!hConnectionEndEvt)
	{
		logger.LogMessage("Unable to create connection end event.");
		return 0;
	}

	logger.LogMessage("Server socket initialized. Will start listening for incoming connections.");
	return 1;
}

DWORD WINAPI ClipShareServerListener::ServerSenderThread(LPVOID lpParam)
{
	return pServerListener->CSServerSenderThread(lpParam);
}

DWORD ClipShareServerListener::CSServerSenderThread(LPVOID lpParam)
{
	SetEvent(hConnectionEndEvt);

	return 1;
}

DWORD WINAPI ClipShareServerListener::ServerReceiverThread(LPVOID lpParam)
{
	return pServerListener->CSServerReceiverThread(lpParam);
}

DWORD ClipShareServerListener::CSServerReceiverThread(LPVOID lpParam)
{
	SetEvent(hConnectionEndEvt);

	return 1;
}

bool ClipShareServerListener::ProcessClient(SOCKET *sockClient)
{
	HANDLE hSenderThread = CreateThread(NULL, 0, ServerSenderThread, NULL, 0, NULL);
	if(!hSenderThread)
		return false;

	HANDLE hReceiverThread = CreateThread(NULL, 0, ServerReceiverThread, NULL, 0, NULL);
	if(!hReceiverThread)
		return false;

	return true;
}

DWORD ClipShareServerListener::CSServerListenerWorkerThread(LPVOID lpParam)
{
	SOCKET sockClient;
	struct sockaddr saClient;
	int iSAClientLen;

	std::string sFailedAcceptErrorMsgText = "Received connection from client, but an error occurred. Going to continue listening for connections. WSA error code: ";

	logger.LogMessage("Server waiting for incoming connections...");

	while(!bServiceStopping)
	{
		iSAClientLen = sizeof(saClient);
		ZeroMemory((void *)&saClient, iSAClientLen);

		sockClient = accept(sockServer, &saClient, &iSAClientLen);
		if(sockClient == INVALID_SOCKET)
		{
			std::ostringstream ossFailedAcceptErrorMsg;
			ossFailedAcceptErrorMsg<<sFailedAcceptErrorMsgText<<WSAGetLastError();
			logger.LogMessage(ossFailedAcceptErrorMsg.str());

			continue;
		}
		else
		{
			ResetEvent(hConnectionEndEvt);

			if(!ProcessClient(&sockClient))
				logger.LogMessage("Error in creating the sender or receiver thread. Try connecting again. Going back to listening for connections.");
			else
			{
				HANDLE hWaitHandles[2] = {hServiceStopEvt, hConnectionEndEvt};
				WaitForMultipleObjects(2, hWaitHandles, FALSE, INFINITE);

				if(hConnectionEndEvt == INVALID_HANDLE_VALUE || WaitForSingleObject(hServiceStopEvt, 0) != WAIT_TIMEOUT)
					break;
			}
		}
	}

	logger.LogMessage("Shutting down listener worker thread.");
	return 1;
}

DWORD WINAPI ClipShareServerListener::ServerListenerWorkerThread(LPVOID lpParam)
{
	return pServerListener->CSServerListenerWorkerThread(lpParam);
}

DWORD ClipShareServerListener::CSServerListenerThread(LPVOID lpParam)
{
	hServerListenerWorkerThread = CreateThread(NULL, 0, ServerListenerWorkerThread, NULL, 0, NULL);
	if(!hServerListenerWorkerThread)
	{
		logger.LogMessage("Could not initialize server listener worker thread. Try restarting the service.");
		return 0;
	}
	logger.LogMessage("Created server listener worker thread.");
	
	WaitForSingleObject(hServiceStopEvt, INFINITE);
	bServiceStopping = true;
	CleanServerListener();
	
	return 1;
}

DWORD WINAPI ClipShareServerListener::ServerListenerThread(LPVOID lpParam)
{
	return pServerListener->CSServerListenerThread(lpParam);
}

DWORD ClipShareServerListener::InitServerListener()
{
	if(!InitServerListenerWorker())
	{
		logger.LogMessage("Unable to initialize socket. Service will stop.");
		CleanServerListener();
		return 0;
	}

	pServerListener = this;

	hServiceStopEvt = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!hServiceStopEvt)
	{
		logger.LogMessage("Unable to create service stop event. Service will stop.");
		return 0;
	}
	logger.LogMessage("Created service stop event.");

	hServerListenerThread = CreateThread(NULL, 0, ServerListenerThread, NULL, 0, NULL);
	if(!hServerListenerThread)
	{
		logger.LogMessage("Unable to start server listener thread. Service will stop.");
		return 0;
	}
	logger.LogMessage("Created server listener thread.");

	return 1;
}

void ClipShareServerListener::CleanServerListener()
{
	closesocket(sockServer);
	WSACleanup();
	logger.LogMessage("Server socket closed.");

	WaitForSingleObject(hConnectionEndEvt, INFINITE);

	//what if the below part is executed before ServerListenerWorkerThread can check its handle and terminate?
	CloseHandle(hServiceStopEvt);
	CloseHandle(hConnectionEndEvt);
	logger.LogMessage("Handles cleaned up.");
}