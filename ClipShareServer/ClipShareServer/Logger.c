#include <stdio.h>

#include "Logger.h"

#define	CUR_TIME_MAX_LENGTH	20

HANDLE hLogger = NULL;

void InitLogger(LPCSTR strLogFile)
{
	hLogger = CreateFileA(strLogFile, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if(hLogger == INVALID_HANDLE_VALUE)
	{
		hLogger = NULL;
	}
}

void LogMessage(LPCSTR strErrorLogMsg)
{
	if(hLogger)
	{
		SYSTEMTIME stCurTime;
		LPSTR strCurTime;
		DWORD dwNumberOfBytesWritten;

		GetSystemTime(&stCurTime);

		strCurTime = (LPSTR)GlobalAlloc(GMEM_FIXED, CUR_TIME_MAX_LENGTH*sizeof(char));

		sprintf(strCurTime, "[%hu/%hu/%hu %hu:%hu:%hu] ", stCurTime.wDay, stCurTime.wMonth, stCurTime.wYear, stCurTime.wHour, stCurTime.wMinute, stCurTime.wSecond);
		
		WriteFile(hLogger, strCurTime, lstrlenA(strCurTime), &dwNumberOfBytesWritten, NULL);
		WriteFile(hLogger, strErrorLogMsg, lstrlenA(strErrorLogMsg), &dwNumberOfBytesWritten, NULL);
		WriteFile(hLogger, "\n", 1, &dwNumberOfBytesWritten, NULL);

		GlobalFree(strCurTime);
	}
}

void CloseLogger()
{
	if(hLogger != NULL)
	{
		CloseHandle(hLogger);
		hLogger = NULL;
	}
}