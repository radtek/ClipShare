#include "Logger.h"
#include <sstream>

void Logger::InitLogger(std::string sLogFileName)
{
	hLogger = CreateFileA(sLogFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if(hLogger == INVALID_HANDLE_VALUE)
	{
		hLogger = NULL;
	}
	else
	{
		hFileLock = CreateMutex(NULL, FALSE, NULL);
	}
}

void Logger::LogMessage(std::string sLogMsg)
{
	if(hLogger)
	{
		switch(WaitForSingleObject(hFileLock, INFINITE))
		{
			case WAIT_OBJECT_0: 	SYSTEMTIME stCurTime;
									DWORD dwNumberOfBytesWritten;

									GetSystemTime(&stCurTime);

									std::ostringstream ssCurTime;
									ssCurTime<<"["<<stCurTime.wDay<<"/"
												<<stCurTime.wMonth<<"/"
												<<stCurTime.wYear<<" "
												<<stCurTime.wHour<<":"
												<<stCurTime.wMinute<<":"
												<<stCurTime.wSecond<<"] ";
		
									std::string tempCurTime = ssCurTime.str();
									LPCSTR strCurTime = tempCurTime.c_str();
									LPCSTR strLogMsg = sLogMsg.c_str();

									WriteFile(hLogger, strCurTime, lstrlenA(strCurTime), &dwNumberOfBytesWritten, NULL);
									WriteFile(hLogger, strLogMsg, lstrlenA(strLogMsg), &dwNumberOfBytesWritten, NULL);
									WriteFile(hLogger, "\n", 1, &dwNumberOfBytesWritten, NULL);
							
									ReleaseMutex(hFileLock);
									break;
		}
	}
}

void Logger::CloseLogger()
{
	if(hLogger != NULL)
	{
		CloseHandle(hLogger);
		hLogger = NULL;

		if(hFileLock != NULL)
		{
			CloseHandle(hFileLock);
			hFileLock = NULL;
		}
	}
}