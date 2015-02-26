#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Shlwapi.h>
#include <sstream>
#include <algorithm>

#include "ClipShareServerListener.h"

ClipShareServerListener *ClipShareServerListener::pServerListener = NULL;

ClipShareServerListener::ClipShareServerListener(Logger *_logger)
{
	logger = *_logger;
	
	hServiceStopEvt = NULL;
	bServiceStopping = false;
}

void ClipShareServerListener::SetServiceStopEvt()
{
	SetEvent(hServiceStopEvt);
}

int ClipShareServerListener::InitServerListenerWorker()
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

	hSenderStopEvt = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!hSenderStopEvt)
	{
		logger.LogMessage("Unable to create sender thread stop event.");
		return 0;
	}
	
	hReceiverStopEvt = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!hReceiverStopEvt)
	{
		logger.LogMessage("Unable to create receiver thread stop event.");
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
	SOCKET client = *(SOCKET *)lpParam;

	while(!bServiceStopping)
	{
		if(send(client, &CONNECTION_ALIVE_MSG, sizeof(CONNECTION_ALIVE_MSG), 0) != SOCKET_ERROR)
		{
			Sleep(ALIVE_MSG_INTERVAL_SEC * 1000);
			continue;
		}
		else
		{
			DWORD dwLastSockError = WSAGetLastError();

			switch(dwLastSockError)
			{
				case WSAENOTCONN:
				case WSAENOTSOCK: logger.LogMessage("Connection to client is no longer valid or the socket has been closed. Exiting server sender thread.");
								  break;
				default: std::ostringstream ossSocketError;
						 ossSocketError<<"Error in sending alive message to client. Exiting server sender thread. WSA error code: "<<WSAGetLastError();
						 logger.LogMessage(ossSocketError.str());
						 break;
			}

			break;
		}
	}

	SetEvent(hSenderStopEvt);

	return 1;
}

int ClipShareServerListener::PostToClipboard(std::string sClipText)
{
	BOOL bClipRet = OpenClipboard(NULL);
	if(!bClipRet)
	{
		logger.LogMessage("Unable to open clipboard for posting data.");
		return 1;
	}

	bClipRet = EmptyClipboard();
	if(!bClipRet)
	{
		logger.LogMessage("Unable to empty clipboard.");
		return 1;
	}

	int iTextLength = sClipText.length();
	HGLOBAL hTextBuffer = GlobalAlloc(GMEM_FIXED, iTextLength + 1);
	ZeroMemory(hTextBuffer, iTextLength + 1);
	memcpy((void *)hTextBuffer, (void *)sClipText.c_str(), iTextLength);

	HANDLE hClipData = SetClipboardData(CF_TEXT, hTextBuffer);

	GlobalFree(hTextBuffer);
	CloseClipboard();
	
	if(hClipData == NULL)
	{
		logger.LogMessage("Unable to set the clipboard data.");
		return 1;
	}

	return 0;
}

DWORD WINAPI ClipShareServerListener::ServerReceiverThread(LPVOID lpParam)
{
	return pServerListener->CSServerReceiverThread(lpParam);
}

DWORD ClipShareServerListener::CSServerReceiverThread(LPVOID lpParam)
{
	SOCKET client = *(SOCKET *)lpParam;

	char inData = '\0';

	while(!bServiceStopping)
	{
		int iRecvCode = recv(client, &inData, sizeof(inData), 0);

		if(iRecvCode != SOCKET_ERROR)
		{
			if(iRecvCode == 0)
			{
				logger.LogMessage("Connection to client closed. Closing clipboard sharing session.");
				break;
			}
			else
			{
				if(inData == CONNECTION_ALIVE_MSG)
					continue;
				else if(inData == CONNECTION_DATA_MSG)
				{
					int iDataLength = 0;
					char cDelim;
					bool bContinue = true;

					if(recv(client, &cDelim, sizeof(char), 0) != SOCKET_ERROR && cDelim == 'L')
					{
						char szDataLengthBuf[100] = {'\0'};
						char cCurChar;
						int iBufIndex = 0;
						do
						{
							iRecvCode = recv(client, &cCurChar, sizeof(char), 0);

							if(iRecvCode == SOCKET_ERROR || cCurChar == 'L')
								break;
							szDataLengthBuf[iBufIndex++] = cCurChar;
						}while(iBufIndex < 100);

						if(iRecvCode == SOCKET_ERROR || iBufIndex >= 100)
						{
							logger.LogMessage("Error in getting length of data from client.");
							bContinue = false;
							break;
						}

						std::string sDataBuf(szDataLengthBuf, iBufIndex);
						iDataLength = std::stoi(sDataBuf);

						if(iDataLength > 0)
						{
							char *szData = new char[iDataLength + 1];
							std::fill(szData, szData + iDataLength + 1, '\0');

							int iDataIndex;
							for(iDataIndex = 0;iDataIndex < iDataLength;iDataIndex++)
								if(recv(client, &cCurChar, sizeof(cCurChar), 0) != SOCKET_ERROR)
									szData[iDataIndex] = cCurChar;
								else
									break;

							if(iDataIndex != iDataLength)
							{
								logger.LogMessage("Error in reading clipboard data from client.");
								bContinue = false;
							}
							else
							{
								std::ostringstream ossClipData;
								ossClipData<<"Received clipboard data from client: "<<szData;
								logger.LogMessage(ossClipData.str());

								if(PostToClipboard(std::string(szData)))
									logger.LogMessage("An error occurred while posting data to the clipboard. No data posted.");
							}

							delete[] szData;
						}
					}
					else
					{
						logger.LogMessage("Error in getting data length delimiter.");
						bContinue = false;
					}

					if(!bContinue)
					{
						logger.LogMessage("An error occurred in data transfer. Closing clipboard sharing session.");
						break;
					}
				}
				else
				{
					logger.LogMessage("Received unknown message from client. Closing clipboard sharing session.");
					break;
				}
			}
		}
		else
		{
			DWORD dwLastSockError = WSAGetLastError();

			switch(dwLastSockError)
			{
				case WSAETIMEDOUT: logger.LogMessage("Connection to client timed out. Closing clipboard sharing session.");
								   break;
				default: std::ostringstream ossSocketError;
						 ossSocketError<<"Error in receiving data from client. Exiting server receiver thread. WSA error code: "<<WSAGetLastError();
						 logger.LogMessage(ossSocketError.str());
						 break;
			}

			break;
		}
	}

	SetEvent(hReceiverStopEvt);

	return 1;
}

int ClipShareServerListener::PerformHandshake(SOCKET client)
{
	if(send(client, &HANDSHAKE_MSG, sizeof(HANDSHAKE_MSG), 0) == SOCKET_ERROR)
	{
		logger.LogMessage("Error in sending handshake message. Exiting...");
		return 1;
	}

	char inData;
	if(recv(client, &inData, 1, 0) == SOCKET_ERROR || inData != HANDSHAKE_MSG)
	{
		logger.LogMessage("Error in receiving handshake message. Exiting...");
		return 1;
	}

	return 0;
}

bool ClipShareServerListener::ProcessClient(SOCKET *sockClient)
{
	if(!PerformHandshake(*sockClient))
	{
		HANDLE hSenderThread = CreateThread(NULL, 0, ServerSenderThread, (void *)sockClient, 0, NULL);
		if(!hSenderThread)
			return false;

		HANDLE hReceiverThread = CreateThread(NULL, 0, ServerReceiverThread, (void *)sockClient, 0, NULL);
		if(!hReceiverThread)
			return false;
	}
	else
		return false;

	return true;
}

DWORD ClipShareServerListener::CSServerListenerWorkerThread(LPVOID lpParam)
{
	SOCKET sockClient;
	struct sockaddr_in saClient;
	int iSAClientLen;

	std::string sFailedAcceptErrorMsgText = "Received connection from client, but an error occurred. Going to continue listening for connections. WSA error code: ";

	logger.LogMessage("Server waiting for incoming connections...");

	while(!bServiceStopping && WaitForSingleObject(hServiceStopEvt, 0) != WAIT_OBJECT_0)
	{
		iSAClientLen = sizeof(saClient);
		ZeroMemory((void *)&saClient, iSAClientLen);

		sockClient = accept(sockServer, (sockaddr *)&saClient, &iSAClientLen);
		if(sockClient == INVALID_SOCKET)
		{
			std::ostringstream ossFailedAcceptErrorMsg;
			ossFailedAcceptErrorMsg<<sFailedAcceptErrorMsgText<<WSAGetLastError();
			logger.LogMessage(ossFailedAcceptErrorMsg.str());

			continue;
		}
		else
		{
			std::ostringstream ossConnectionReceived;
			ossConnectionReceived<<"Received connection from Android device with IP: "<<inet_ntoa(saClient.sin_addr);
			logger.LogMessage(ossConnectionReceived.str());

			setsockopt(sockClient, SOL_SOCKET, SO_RCVTIMEO, (char *)&READ_TIMEOUT, sizeof(int));

			if(!ProcessClient(&sockClient))
				logger.LogMessage("Error in creating the sender or receiver thread. Try connecting again. Going back to listening for connections.");
			else
			{
				logger.LogMessage("Handshake complete, connection established.");

				ResetEvent(hSenderStopEvt);
				ResetEvent(hReceiverStopEvt);

				HANDLE hAllWaitHandles[3] = {hServiceStopEvt, hSenderStopEvt, hReceiverStopEvt};
				WaitForMultipleObjects(3, hAllWaitHandles, FALSE, INFINITE);

				closesocket(sockClient);

				HANDLE hClientWaitHandles[2] = {hSenderStopEvt, hReceiverStopEvt};
				WaitForMultipleObjects(2, hClientWaitHandles, TRUE, INFINITE);

				ossConnectionReceived.str(std::string());
				ossConnectionReceived<<"Lost connection to: "<<inet_ntoa(saClient.sin_addr);
				logger.LogMessage(ossConnectionReceived.str());

				if(WaitForSingleObject(hServiceStopEvt, 0) != WAIT_TIMEOUT)
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

	//what if the below part is executed before ServerListenerWorkerThread can check its handle and terminate?
	CloseHandle(hServiceStopEvt);
	CloseHandle(hSenderStopEvt);
	CloseHandle(hReceiverStopEvt);
	logger.LogMessage("Handles cleaned up.");
}
