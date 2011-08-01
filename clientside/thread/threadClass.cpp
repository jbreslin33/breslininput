#include "ThreadClass.h"

#include "../../tdreamsock/dreamSockLog.h"
#include "../game/game.h"

ThreadClass::ThreadClass() { } // Constructor
ThreadClass::~ThreadClass() { } // Destructor

void ThreadClass::RunTheNetwork(Game* game) {
    // Thread execution stuff goes here
	int i = 0;
	while(true)
	{
		game->ReadPackets();
		//boost::this_thread::sleep(boost::posix_time::milliseconds(32));
	}

}

void ThreadClass::RunTheGame(Game* game) {
    // Thread execution stuff goes here
    game->go();
	while(true)
	{
		game->gameLoop();
	}
}