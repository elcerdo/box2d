#include "gamehelper.h"

UserData::UserData(b2Body* body) : body(body) {}
Ball::Ball(b2Body* body) : UserData(body), nhit(0) {}

Team::Team(const Team::Field &field) : field(field), score(0);
int Team::getScore() const { return score; }
int Team::teamScored() { score++; }

Player::Player(b2Body* body, Team &team) : UserData(body), team(team), jumping(false), jump_speed(0), jump_time(-1) {}
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

