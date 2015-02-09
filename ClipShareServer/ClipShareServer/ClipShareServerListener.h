#ifndef CLIP_SHARE_SERVER_LISTENER_HEADER
#define	CLIP_SHARE_SERVER_LISTENER_HEADER

#include "Logger.h"
#include <Windows.h>

class ClipShareServerListener
{
	private:
		HANDLE hServerListenerThread, hServerListenerWorkerThread;
		SOCKET sockServer;
		Logger logger;

		DWORD InitServerListenerWorker();
		bool ProcessClient(SOCKET *);
		int PerformHandshake(SOCKET client);

		static DWORD WINAPI ServerListenerThread(LPVOID);
		static DWORD WINAPI ServerListenerWorkerThread(LPVOID);

		static DWORD WINAPI ServerSenderThread(LPVOID);
		static DWORD WINAPI ServerReceiverThread(LPVOID);	

		static const int SERVER_LISTENER_PORT = 7489;
		static const int CONNECTION_MSG_MAX_BUFFER = 1024;
		static const int READ_TIMEOUT = 10 * 1000;
		static const int ALIVE_MSG_INTERVAL_SEC = 5;

		static const char HANDSHAKE_MSG = 0x12;
		static const char CONNECTION_ALIVE_MSG = 0x23;
		static const char CONNECTION_DATA_MSG = 0x34;
		static const char CONNECTION_END_MSG = 0x45;

		HANDLE hServiceStopEvt;
		HANDLE hConnectionEndEvt;
		bool bServiceStopping;

		static ClipShareServerListener *pServerListener;

	public:
		ClipShareServerListener(Logger *);

		DWORD InitServerListener();
		void CleanServerListener();
		
		DWORD CSServerListenerThread(LPVOID);
		DWORD CSServerListenerWorkerThread(LPVOID);

		DWORD CSServerSenderThread(LPVOID);
		DWORD CSServerReceiverThread(LPVOID);

		void SetServiceStopEvt();
};

#endif
