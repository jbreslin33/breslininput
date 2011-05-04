#ifndef SERVERSIDESINBAD_H
#define SERVERSIDESINBAD_H

#define NUM_ANIMS 13           // number of animations the character has
#define CHAR_HEIGHT 5          // height of character's center of mass above ground
#define CAM_HEIGHT 2           // height of camera above character's center of mass
#define RUN_SPEED 17           // character running speed in units per second
#define TURN_SPEED 500.0f      // character turning in degrees per second
#define ANIM_FADE_SPEED 7.5f   // animation crossfade speed in % of full weight per second
#define JUMP_ACCEL 30.0f       // character jump acceleration in upward units per squared second
#define GRAVITY 90.0f          // gravity in downward units per squared second

#include "../player/player.h"
#include "../client/serverSideClient.h"

class ServerSidePlayer : public Player
{

public:

ServerSidePlayer(ServerSideClient* client, Shape* shape, std::string name);
~ServerSidePlayer();

//movement
void processTick();
void calculateVelocity(Command *command, float frametime);

//associated client
ServerSideClient* mClient;

};

#endif

