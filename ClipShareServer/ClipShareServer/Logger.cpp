#include "Logger.h"
#include <sstream>

void Logger::InitLogger(std::string sLogFileName)
{
	hLogger = CreateFileA(sLogFileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if(hLogger == INVALID_HANDLE_VALUE)
	{
		hLogger = NULL;
	}
}

void Logger::LogMessage(std::string sLogMsg)
{
	if(hLogger)
	{
		SYSTEMTIME stCurTime;
		DWORD dwNumberOfBytesWritten;

		GetSystemTime(&stCurTime);

		std::stringstream ssCurTime;
		ssCurTime<<"["<<stCurTime.wDay<<"/"
				 <<stCurTime.wMonth<<"/"
				 <<stCurTime.wYear<<" "
				 <<stCurTime.wHour<<"/"
				 <<stCurTime.wMinute<<"/"
				 <<stCurTime.wSecond<<"] ";

		LPCSTR strCurTime = ssCurTime.str().c_str();
		LPCSTR strLogMsg = sLogMsg.c_str();

		WriteFile(hLogger, strCurTime, lstrlenA(strCurTime), &dwNumberOfBytesWritten, NULL);
		WriteFile(hLogger, strLogMsg, lstrlenA(strLogMsg), &dwNumberOfBytesWritten, NULL);
		WriteFile(hLogger, "\n", 1, &dwNumberOfBytesWritten, NULL);
	}
}

void Logger::CloseLogger()
{
	if(hLogger != NULL)
	{
		CloseHandle(hLogger);
		hLogger = NULL;
	}
}