#ifndef __GAMEDATA_H__
#define __GAMEDATA_H__

#include <QObject>
#include "world.h"

class GameData : public QObject {
    Q_OBJECT
public:
    GameData(World& world, QObject* parent=NULL);

    void leftPlayerStart();
    void rightPlayerStart();

    void leftPlayerGoLeft();
    void leftPlayerGoRight();
    void leftPlayerStopLeft();
    void leftPlayerStopRight();
    void leftPlayerJump();
    void leftPlayerStopJump();
    void rightPlayerGoLeft();
    void rightPlayerGoRight();
    void rightPlayerStopLeft();
    void rightPlayerStopRight();
    void rightPlayerJump();
    void rightPlayerStopJump();

public slots:
    void stabilizePlayers(World*);
    void pointMarked();

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
    int score_right_player;
    int score_left_player;
    bool right_player_jumping;
    bool left_player_jumping;
};

#endif

