#ifndef __GAMEHELPER_H__
#define __GAMEHELPER_H__

#include "common.h"
#include <QList>
#include <QPixmap>

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
    void clearPositions();
    void recordPosition();
    void drawPositions(QPixmap& pixmap) const;
protected:
    struct Position {
	float x,y;
    };
    typedef QList<Position> Positions;
    Positions positions;
    //int nhit;
};

class Bird : public UserData {
public:
    Bird(b2Body* body);
};

class Team {
public:
    enum Field {LEFT,RIGHT};
    Team(const Field &field);
    Field getField() const;
    int getScore() const;
    void teamScored();
    void resetScore();
    int getSet() const;
    void teamWonSet();
    QString getName() const;
protected:
    const Field field;
    int score;
    int set;
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
    const Team &getTeam() const;
    QString getName() const;
protected:
    Team &team;
    bool jumping;
    float jump_speed;
    float jump_time;
};

#endif
