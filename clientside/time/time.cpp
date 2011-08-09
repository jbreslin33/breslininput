#include "time.h"

#ifdef WIN32
#include "../../tdreamsock/dreamWinSock.h"
#include <windows.h>
#else
#include "../tdreamsock/dreamLinuxSock.h"
#endif

Time::Time()
{
	#ifdef WIN32
		mDreamWinSock = new DreamWinSock();
	#else
		mDreamLinuxSock = new DreamLinuxSock();
	#endif
}

Time::~Time()
{
}


//TIME
int Time::dreamSock_GetCurrentSystemTime(void)
{
	#ifndef WIN32
		return dreamSock_Linux_GetCurrentSystemTime();
	#else
		return dreamSock_Win_GetCurrentSystemTime();
	#endif
}

int Time::dreamSock_Win_GetCurrentSystemTime(void)
{
	int curtime;
	static int base;
	static bool initialized = false;

	if(!initialized)
	{
		base = timeGetTime() & 0xffff0000;
		initialized = true;
	}

	curtime = timeGetTime() - base;

	return curtime;
}