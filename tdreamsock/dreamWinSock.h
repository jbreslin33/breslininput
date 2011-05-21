#ifndef DREAMWINSOCK_H
#define DREAMWINSOCK_H

class DreamWinSock
{

public:

DreamWinSock();
~DreamWinSock();

int	dreamSock_InitializeWinSock(void);
int dreamSock_Win_GetCurrentSystemTime(void);
};

#endif