#ifndef __GAMEHELPER_H__
#define __GAMEHELPER_H__

#include "common.h"

class UserData {
public:
    UserData(b2Body* body);
    const b2Body* getBody() const;
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
    Field getField() const;
    int getScore() const;
    int teamScored();
protected:
    const Field field;
    int score;
};

class Player : public UserData {
public:
    Player(b2Body* body, Team &team);
    void goLeft(float time);
    void goRight(float time);
    void goJump(float time);
    void stopLeft(float time);
    void stopRight(float time);
    void stopJump(float time);
    void checkPosition(float time);
    void checkJump(float time);
    Team &getTeam();
protected:
    Team &team;
    bool jumping;
    float jump_speed;
    float jump_time;
};

#endif
