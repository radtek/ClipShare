#ifndef CLIP_SHARE_SERVICE_HEADER
#define CLIP_SHARE_SERVICE_HEADER

#include <string>
#include <Windows.h>
#include <winsvc.h>

class CClipShareService
{
	private:
		std::wstring m_serviceName;
		SERVICE_TABLE_ENTRY m_serviceTable[2];
		SERVICE_STATUS m_serviceStatus;
		SERVICE_STATUS_HANDLE m_serviceStatusHandle;

	public:
		static VOID WINAPI ServiceMain(DWORD, LPWSTR *);
		static VOID WINAPI ServiceCtrlHandler(DWORD);
		static DWORD WINAPI ServiceWorker(LPVOID);

		CClipShareService(std::wstring);
		std::wstring GetServiceName();

		bool InitService();

		void ClipShareServiceMain(DWORD, LPWSTR *);
		void ClipShareServiceCtrlHandler(DWORD);
		DWORD ClipShareServiceWorker(LPVOID);
};

#endif