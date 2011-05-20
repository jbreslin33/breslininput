//
// DreamSock network-library
//
// Programmers:
// Teijo Hakala
//
/////////////////////////////////////////////////////////

//#define WIN32

#include "../message/message.h"

#ifdef WIN32
// Windows specific headers
	#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_
	#include <windows.h>
	#include <winsock2.h>

	#endif
#else
// UNIX specific headers
	#include <memory.h>
	#include <errno.h>
	#include <sys/ioctl.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif

// Common headers
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "dreamSock.h"
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

