#include "clientSideClient.h"
#include "../player/clientSidePlayer.h"

ClientSideClient::ClientSideClient()
{
	mIndex  = 0;
	mPlayer = NULL;
	mServerPlayer = NULL;
}

ClientSideClient::~ClientSideClient()
{
}
