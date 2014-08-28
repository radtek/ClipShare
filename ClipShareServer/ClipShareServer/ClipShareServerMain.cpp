#include <Windows.h>
#include <winsvc.h>
#include "ClipShareService.h"

int main()
{
	std::wstring wstrServiceName(L"ClipShare Server");

	CClipShareService csService(wstrServiceName);

	if(!csService.InitService())
		return GetLastError();

	return 0;
}

