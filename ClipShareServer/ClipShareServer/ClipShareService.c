#include <windows.h>

#include "Logger.h"
#include "ClipShareService.h"
#include "ClipShareServerListener.h"

static SERVICE_STATUS serviceStatus;
static SERVICE_STATUS_HANDLE hServiceStatus;
static HANDLE hServiceStopEvt;

HANDLE GetServiceStopEvt()
{
	return hServiceStopEvt;
}

void StopService()
{
	LogMessage("Service shutting down.");
	CloseLogger();						

	SetEvent(hServiceStopEvt);

	serviceStatus.dwControlsAccepted = 0;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(hServiceStatus, &serviceStatus);
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
	hServiceStatus = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);
	
	if(hServiceStatus)
	{
		serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		serviceStatus.dwCurrentState = SERVICE_START_PENDING;
		serviceStatus.dwControlsAccepted = 0;
		serviceStatus.dwWin32ExitCode = 0;
		serviceStatus.dwServiceSpecificExitCode = 0;
		serviceStatus.dwWaitHint = 0;
		serviceStatus.dwCheckPoint = 0;

		if(SetServiceStatus(hServiceStatus, &serviceStatus))
		{
			LogMessage("Service starting.");

			if(!InitServerListener())
			{
				StopService();
			}
			else
			{
				serviceStatus.dwCurrentState = SERVICE_RUNNING;
				serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
				SetServiceStatus(hServiceStatus, &serviceStatus);

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