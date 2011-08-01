#ifndef THREADCLASS_H_
#define THREADCLASS_H_

class Game;

class ThreadClass {
    public:
        ThreadClass(); // Constructor
        ~ThreadClass(); // Destructor
        void RunTheNetwork(Game* game);
		        void RunTheGame(Game* game);
		
};

#endif