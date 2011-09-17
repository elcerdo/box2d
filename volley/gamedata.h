#ifndef __GAMEDATA_H__
#define __GAMEDATA_H__

#include <QObject>
#include "sound.h"
#include "world.h"
#include "gamehelper.h"

class GameData : public QObject {
    Q_OBJECT
public:
    enum State {
	INIT,
	STARTING,
	PLAYING,
	FINISHED
    };
 
    GameData(Sound &sound,World& world, QObject* parent=NULL);
    State getState() const;
    float getLastTransitionTime() const;
    const Team& getLastScoringTeam() const;
    const Player* getLastScoringPlayer() const;
    const Player* getLastTouchingPlayer() const;
    bool isLastTouchingPlayer(const Player& player) const;

    void beginPoint(World& world);

    Ball& getBall();
    Team& getTeam(Team::Field field);
    Player& getPlayer(int number);
    Player& getLeftPlayer();
    Player& getRightPlayer();

public slots:
    void recordBallPosition(World*);
    void stabilizePlayers(World*);
    void checkState(World*);

protected:
    void buildCourt(World& world);
    void updateScoreAndSet(World &world);

    Sound &sound;

    b2Body* body_left_ground;
    b2Body* body_right_ground;
    b2Body* body_left_wall;
    b2Body* body_right_wall;
    b2Body* body_ceiling;
    b2Body* body_left_tack;
    b2Body* body_right_tack;
    b2Body* body_net;
    b2Body* body_ball;
    b2Body* body_left_player;
    b2Body* body_right_player;

    Ball* ball;
    Team* left_team;
    Team* right_team;
    Player* left_player;
    Player* right_player;

    float last_transition_time;
    Team* last_scoring_team;
    Player* last_scoring_player;
    Player* last_touching_player;

    State state;
};

#endif

