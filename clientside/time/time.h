#ifndef TIME_H
#define TIME_H

#ifdef WIN32

class DreamWinSock;
#else
class DreamLinuxSock;
#endif

#ifdef WIN32
class DreamWinSock;
#endif

class Time 
{
public:
	
	Time();
	~Time();

	#ifdef WIN32
	DreamWinSock* mDreamWinSock;
	#else
	DreamLinuxSock* mDreamLinuxSock;
	#endif

	//time
	#ifndef WIN32
	int dreamSock_Linux_GetCurrentSystemTime();
	#endif
	int dreamSock_GetCurrentSystemTime();

	int dreamSock_Win_GetCurrentSystemTime();
};

#endif
