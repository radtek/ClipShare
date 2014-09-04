#include <windows.h>
#include <winsvc.h>

#include "ClipShareService.h"

int main()
{
	DWORD dwServiceInitRetCode = InitService();

	return dwServiceInitRetCode;
}

