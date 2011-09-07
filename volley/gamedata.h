#ifndef __GAMEDATA_H__
#define __GAMEDATA_H__

#include "world.h"

class GameData {
public:
    GameData(World& world);

    void leftPlayerGoLeft();
    void leftPlayerGoRight();
    void leftPlayerStopLeft();
    void leftPlayerStopRight();

protected:
    void buildCourt(World& world);
    b2Body* left_ground;
    b2Body* right_ground;
    b2Body* ceiling;
    b2Body* left_wall;
    b2Body* right_wall;
    b2Body* net;
    b2Body* ball;
    b2Body* left_player;
    b2Body* right_player;

};

#endif

