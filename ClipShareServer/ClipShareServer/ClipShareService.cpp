#include "ClipShareService.h"

CClipShareService *pCsService;

CClipShareService::CClipShareService(std::wstring wstrServiceName)
{
	if(wstrServiceName.size() == 0)
		m_serviceName.assign(L"DefClipShareService");
	else
		m_serviceName.assign(wstrServiceName);

	pCsService = this;
}

std::wstring CClipShareService::GetServiceName()
{
	return m_serviceName;
}

bool CClipShareService::InitService()
{
	SERVICE_TABLE_ENTRY ServiceTable[] = {{(LPWSTR)m_serviceName.c_str(), (LPSERVICE_MAIN_FUNCTION)ServiceMain}
										  , {NULL, NULL}};

	if(!StartServiceCtrlDispatcher(ServiceTable))
		return false;

	return true;
}

VOID WINAPI CClipShareService::ServiceMain(DWORD argc, LPWSTR *argv)
{
	pCsService->ClipShareServiceMain(argc, argv);
}

void CClipShareService::ClipShareServiceMain(DWORD argc, LPWSTR *argv)
{
	m_serviceStatusHandle = RegisterServiceCtrlHandler(m_serviceName.c_str(), ServiceCtrlHandler);
	if(m_serviceStatusHandle)
	{
		m_serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		m_serviceStatus.dwCurrentState = SERVICE_RUNNING;
		m_serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		m_serviceStatus.dwWin32ExitCode = 0;
		m_serviceStatus.dwServiceSpecificExitCode = 0;
		m_serviceStatus.dwWaitHint = 0;
		m_serviceStatus.dwCheckPoint = 0;

		if(SetServiceStatus(m_serviceStatusHandle, &m_serviceStatus))
		{
			CreateThread(NULL, 0, ServiceWorker, NULL, 0, NULL);
		}
	}
}

VOID WINAPI CClipShareService::ServiceCtrlHandler(DWORD dwControlCode)
{
	pCsService->ClipShareServiceCtrlHandler(dwControlCode);
}

void CClipShareService::ClipShareServiceCtrlHandler(DWORD dwControlCode)
{
	switch(dwControlCode)
	{
		case SERVICE_CONTROL_STOP:{
									m_serviceStatus.dwControlsAccepted = 0;
									m_serviceStatus.dwCurrentState = SERVICE_STOPPED;
									SetServiceStatus(m_serviceStatusHandle, &m_serviceStatus);
								  }
								  break;
	}
}

DWORD WINAPI CClipShareService::ServiceWorker(LPVOID lpParam)
{
	return pCsService->ClipShareServiceWorker(lpParam);	
}

DWORD CClipShareService::ClipShareServiceWorker(LPVOID lpParam)
{
	return 0;
}

