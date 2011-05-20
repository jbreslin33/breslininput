#include "dreamSock.h"

#include "../message/message.h"
#include "dreamSockLog.h"

bool dreamSock_init = false;

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
int dreamSock_Initialize(void)
{
	if(dreamSock_init == true)
		return 0;

	dreamSock_init = true;

	StartLog();

#ifdef WIN32
	return dreamSock_InitializeWinSock();
#else
	return 0;
#endif
}

//-----------------------------------------------------------------------------
// Name: empty()
// Desc: 
//-----------------------------------------------------------------------------
void dreamSock_Shutdown(void)
{
	if(dreamSock_init == false)
		return;

	LogString("Shutting down dreamSock");

	dreamSock_init = false;

	StopLog();

#ifdef WIN32
	WSACleanup();
#endif
}

