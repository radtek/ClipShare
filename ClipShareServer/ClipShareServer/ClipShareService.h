#ifndef CLIP_SHARE_SERVICE_HEADER
#define CLIP_SHARE_SERVICE_HEADER

#include <windows.h>
#include <string>

#include "Logger.h"
#include "ClipShareServerListener.h"

class ClipShareService
{
	private:
		SERVICE_STATUS serviceStatus;
		SERVICE_STATUS_HANDLE hServiceStatus;
		Logger logger;

		static VOID WINAPI ServiceMain(DWORD , LPWSTR *);
		static VOID WINAPI ServiceCtrlHandler(DWORD);

	public:
		ClipShareService() {}

		int InitAndStartService();
		void StopService();

		void CSServiceMain(DWORD ,LPWSTR *);
		void CSServiceCtrlHandler(DWORD);
};

#endif