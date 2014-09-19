#ifndef LOGGER_HEADER
#define	LOGGER_HEADER

#include <windows.h>


/* ***Functions*** */

void InitLogger(LPCSTR);
void CloseLogger();

void LogMessage(LPCSTR);

/* *** */

#endif