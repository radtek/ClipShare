#include "ClipShareService.h"

std::wstring ClipShareService::sServiceName = L"ClipShareService";
ClipShareService *ClipShareService::pService = NULL;
ClipShareServerListener *ClipShareService::pServerListener = NULL;

int ClipShareService::InitAndStartService()
{
	logger.InitLogger("d:\\logcsserver.txt");

	SERVICE_TABLE_ENTRY ServiceTable[] = {{(LPWSTR)sServiceName.c_str(), (LPSERVICE_MAIN_FUNCTION)ServiceMain}
										  , {NULL, NULL}};

	pService = this;
	
	if(!StartServiceCtrlDispatcher(ServiceTable))
	{
		logger.LogMessage("Unable to start service.");
		logger.CloseLogger();

		return FALSE;
	}

	return TRUE;
}

void ClipShareService::StopService()
{
	pServerListener->SetServiceStopEvt();

	Sleep(1000);

	logger.LogMessage("Service shutting down.");
	logger.CloseLogger();						

	serviceStatus.dwControlsAccepted = 0;
	serviceStatus.dwCurrentState = SERVICE_STOPPED;
	SetServiceStatus(hServiceStatus, &serviceStatus);
}

void ClipShareService::CSServiceMain(DWORD argc, LPWSTR *argv)
{
	hServiceStatus = RegisterServiceCtrlHandler(sServiceName.c_str(), ServiceCtrlHandler);
	
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
			logger.LogMessage("Service starting.");

			pServerListener = new ClipShareServerListener(&logger);
			if(!pServerListener->InitServerListener())
			{
				StopService();
			}
			else
			{
				serviceStatus.dwCurrentState = SERVICE_RUNNING;
				serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
				SetServiceStatus(hServiceStatus, &serviceStatus);

				logger.LogMessage("Service started.");
			}
		}
		else
		{
			logger.LogMessage("Failed to set service to running state.");
			logger.CloseLogger();
		}
	}
}

VOID WINAPI ClipShareService::ServiceMain(DWORD argc, LPWSTR *argv)
{
	pService->CSServiceMain(argc, argv);
}

void ClipShareService::CSServiceCtrlHandler(DWORD dwControlCode)
{
	switch(dwControlCode)
	{
		case SERVICE_CONTROL_STOP:{
									StopService();
								  }
								  break;
	}
}

VOID WINAPI ClipShareService::ServiceCtrlHandler(DWORD dwControlCode)
{
	pService->CSServiceCtrlHandler(dwControlCode);
}