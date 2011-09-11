#ifndef __GAMEHELPER_H__
#define __GAMEHELPER_H__

#include "common.h"

class UserData {
public:
    UserData(b2Body* body);
protected:
    b2Body* body;
};

class Ball : public UserData {
public:
    Ball(b2Body* body);
protected:
    int nhit;
};

class Team {
public:
    enum Field {LEFT,RIGHT};
    Team(const Field &field);
    int getScore() const;
    int teamScored();
protected:
    const Field field;
    int score;
};

class Player : public UserData {
public:
    Player(b2Body* body, Team &team);
    void goLeft();
    void goRight();
    void goJump();
    void stopLeft();
    void stopRight();
    void stopJump();
protected:
    Team &team;
    bool jumping;
    float jump_speed;
    float jump_time;
};

#endif
