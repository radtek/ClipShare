#ifndef CLIP_SHARE_SERVICE_HEADER
#define CLIP_SHARE_SERVICE_HEADER

#include <windows.h>


/* ***DEFINES*** */

#define SERVICE_NAME			TEXT("ClipShareService")
#define SERVICE_INIT_SUCCESS	0x0000000

/* *** */


/* ***Globals*** */

SERVICE_STATUS serviceStatus;
SERVICE_STATUS_HANDLE serviceStatusHandle;

/* *** */


/* ***Functions*** */

DWORD InitService();

VOID WINAPI ServiceMain(DWORD, LPWSTR *);
VOID WINAPI ServiceCtrlHandler(DWORD);
DWORD WINAPI ServiceWorker(LPVOID);

/* *** */

#endif