
#ifdef WIN32
#include <windows.h>
#include <commctrl.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#include "dreamSockLog.h"

FILE *LogFile;

#ifdef WIN32
dreamConsole *console = NULL;
#endif

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void StartLog(void)
{
	time_t current = time(NULL);

	if((LogFile = fopen("dreamSock.log", "w")) != NULL)
	{
		fprintf(LogFile, "Log file started %s", ctime(&current));
		
		fclose(LogFile);
	}

	if((LogFile = fopen("dreamSock.log", "a")) != NULL)
	{
	}
}

#ifdef WIN32
void StartLogConsole(void)
{
	console = new dreamConsole("dreamSock application");
}
#endif

void StopLog(void)
{
	fclose(LogFile);

#ifdef WIN32
	if(console != NULL)
		delete console;
#endif
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void LogString(const char *string, ...)
{
	char buf[1024];
	va_list ap;
	va_start(ap, string);
	size_t t = 1024;
	vsprintf_s(buf, t, string, ap);
	va_end(ap);

	// Get current time and date
	time_t current = time(NULL);

	char timedate[64];
	size_t tt = 64;
	sprintf_s(timedate, tt,ctime(&current));

	// Remove linefeed from time / date string
	int i = 0;

	while(timedate[i] != '\n')
	{
		i++;
	}

	timedate[i] = '\0';

	// Output log string
#ifdef WIN32
	fprintf(LogFile, "%s: %s\n", timedate, buf);

	if(console)
		console->println(buf, 0);
#else
	// Linux outputs to screen and to the open file when running as daemon
	printf("%s: %s\n", timedate, buf);
#endif
}

/***************

 Console

****************/

#ifdef WIN32

dreamConsole::dreamConsole(char *title)
{
	AllocConsole();
	SetConsoleTitle(title);
}

dreamConsole::~dreamConsole()
{
	FreeConsole();
}

void dreamConsole::println(char *string, int type, ...)
{
	char buf[1024];
	char buf2[1024];
	va_list ap;
	va_start(ap, string);
	size_t t = 1024;
	vsprintf_s(buf, t, string, ap);
	va_end(ap);

	sprintf_s(buf2,t, "-> %s\n", buf);

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	switch(type)
	{
	case 0:
		SetConsoleTextAttribute(console, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;

	case 1:
		SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	}

	WriteConsole(console, buf2, strlen(buf2), NULL, NULL);
}

#endif
