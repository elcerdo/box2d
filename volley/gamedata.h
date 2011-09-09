#ifndef __GAMEDATA_H__
#define __GAMEDATA_H__

#include <QObject>
#include "world.h"

class GameData : public QObject {
    Q_OBJECT
public:
  enum state{
    PLAYING,
    STARTPOINT,
    ENDPOINT
  };
 
    GameData(World& world, QObject* parent=NULL);

    void beginPoint();
    
    void leftPlayerStart();
    void rightPlayerStart();

    void leftPlayerGoLeft();
    void leftPlayerGoRight();
    void leftPlayerStopLeft();
    void leftPlayerStopRight();
    void leftPlayerJump(float time);
    void leftPlayerStopJump();
    void rightPlayerGoLeft();
    void rightPlayerGoRight();
    void rightPlayerStopLeft();
    void rightPlayerStopRight();
    void rightPlayerJump(float time);
    void rightPlayerStopJump();

    int leftPlayerScore() const;
    int rightPlayerScore() const;

    float sceneHeight() const;
    float courtWidth() const;
    float courtHeight() const;
    float netHeight() const;
    float netWidth() const;
    float ballRadius() const;
    float playerRadius() const;

    const b2Body* getBall() const;
    const b2Body* getLeftPlayer() const;
    const b2Body* getRightPlayer() const;

public slots:
    void stabilizePlayers(World*);
    void checkPoints(World*);

protected:
    void buildCourt(World& world);
    b2Body* left_ground;
    b2Body* right_ground;
    b2Body* ceiling;
    b2Body* left_wall;
    b2Body* right_wall;
    b2Body* left_tack;
    b2Body* right_tack;
    b2Body* net;
    b2Body* ball;
    b2Body* left_player;
    b2Body* right_player;
    int score_right_player;
    int score_left_player;
    bool right_player_jumping;
    bool left_player_jumping;
    float right_player_jump_speed;
    float left_player_jump_speed;
    float left_player_jump_time;
    float right_player_jump_time;
    state current_state;
};

#endif

