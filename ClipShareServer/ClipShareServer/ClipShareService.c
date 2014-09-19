#include <windows.h>

#include "Logger.h"
#include "ClipShareService.h"

extern DWORD InitServerListener();

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle;

VOID WINAPI ServiceCtrlHandler(DWORD dwControlCode)
{
	switch(dwControlCode)
	{
		case SERVICE_CONTROL_STOP:{
									LogMessage("Service shutting down.");
									CloseLogger();						

									serviceStatus.dwControlsAccepted = 0;
									serviceStatus.dwCurrentState = SERVICE_STOPPED;
									SetServiceStatus(serviceStatusHandle, &serviceStatus);
								  }
								  break;
	}
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPWSTR *argv)
{
	serviceStatus;
	serviceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);
	
	if(serviceStatusHandle)
	{
		serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		serviceStatus.dwCurrentState = SERVICE_RUNNING;
		serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwServiceSpecificExitCode = 0;
		serviceStatus.dwWaitHint = 0;
		serviceStatus.dwCheckPoint = 0;

		if(SetServiceStatus(serviceStatusHandle, &serviceStatus))
		{
			if(!InitServerListener())
			{
				LogMessage("");
			}
		}
		else
		{
			LogMessage("");
		}
	}
}

BOOL InitAndStartService()
{
	SERVICE_TABLE_ENTRY ServiceTable[] = {{SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain}
										  , {NULL, NULL}};

	InitLogger("e:\\logcsserver.txt");

	if(!StartServiceCtrlDispatcher(ServiceTable))
	{
		LogMessage("Unable to start service.");
		CloseLogger();

		return FALSE;
	}

	return TRUE;
}

DWORD WINAPI ServiceWorker(LPVOID lpParam)
{
	return 0;
}