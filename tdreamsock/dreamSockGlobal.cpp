#include "dreamSock.h"

#ifdef WIN32
// Windows specific headers
	#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_
	#endif
	#include <windows.h>
	#include <winsock2.h>
#else
// UNIX specific headers
	#include <memory.h>
	#include <errno.h>
	#include <unistd.h>
	#include <sys/ioctl.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
#endif

#include <stdlib.h>
#include <stdio.h>

