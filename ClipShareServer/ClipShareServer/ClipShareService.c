#include <windows.h>

#include "Logger.h"
#include "ClipShareService.h"

extern DWORD InitServerListener();

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle;

void StopService()
{
	LogMessage("Service shutting down.");
	CloseLogger();						

	serviceStatus.dwControlsAccepted = 0;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(serviceStatusHandle, &serviceStatus);
}

VOID WINAPI ServiceCtrlHandler(DWORD dwControlCode)
{
	switch(dwControlCode)
	{
		case SERVICE_CONTROL_STOP:{
									StopService();
								  }
								  break;
	}
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPWSTR *argv)
{
	serviceStatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);
	
	if(serviceStatusHandle)
	{
		serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		serviceStatus.dwCurrentState = SERVICE_START_PENDING;
		serviceStatus.dwControlsAccepted = 0;
		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwServiceSpecificExitCode = 0;
		serviceStatus.dwWaitHint = 0;
		serviceStatus.dwCheckPoint = 0;

		if(SetServiceStatus(serviceStatusHandle, &serviceStatus))
		{
			LogMessage("Service starting.");

			if(!InitServerListener())
			{
				LogMessage("Unable to start server listener thread. Service will not start.");
				StopService();
			}
			else
			{
				serviceStatus.dwCurrentState = SERVICE_RUNNING;
				serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
				SetServiceStatus(serviceStatusHandle, &serviceStatus);

				LogMessage("Service started.");
			}
		}
		else
		{
			LogMessage("Failed to set service to running state.");
			CloseLogger();
		}
	}
}

BOOL InitAndStartService()
{
	SERVICE_TABLE_ENTRY ServiceTable[] = {{SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain}
										  , {NULL, NULL}};

	InitLogger("d:\\logcsserver.txt");

	if(!StartServiceCtrlDispatcher(ServiceTable))
	{
		LogMessage("Unable to start service.");
		CloseLogger();

		return FALSE;
	}

	return TRUE;
}