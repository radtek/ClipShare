#ifndef LOGGER_HEADER
#define	LOGGER_HEADER

#include <windows.h>
#include <string>

class Logger
{
	private:
		HANDLE hLogger;
		static const int CUR_TIME_MAX_LENGTH = 20;

	public:
		Logger() {}

		void InitLogger(std::string);
		void LogMessage(std::string);
		void CloseLogger();
};

#endif