#include "gamedata.h"

#include "gamemanager.h"
#include <QDebug>

GameData::GameData(World& world, QObject* parent) 
    : QObject(parent), last_transition_time(world.getTime()), last_scoring_team(NULL), state(INIT)
{
    buildCourt(world);
    beginPoint(world);
}

GameData::State GameData::getState() const { return state; }
float GameData::getLastTransitionTime() const { return last_transition_time; }
const Team& GameData::getLastScoringTeam() const { Q_ASSERT(last_scoring_team); return *last_scoring_team; }
Ball& GameData::getBall() { return *ball; }
Team& GameData::getTeam(Team::Field field) { return field==Team::RIGHT ? *right_team : *left_team; }
Player& GameData::getPlayer(int number) { return number ? *right_player : *left_player; }
Player& GameData::getLeftPlayer() { return *left_player; }
Player& GameData::getRightPlayer() { return *right_player; }

void GameData::buildCourt(World &world)
{
    body_left_ground = world.addStaticBox(-GameManager::courtWidth()/4.-.5,-.5,GameManager::courtWidth()/2.+1,1);
    body_left_ground->SetUserData(NULL);
    body_right_ground = world.addStaticBox(GameManager::courtWidth()/4.+.5,-.5,GameManager::courtWidth()/2.+1,1);
    body_right_ground->SetUserData(NULL);
    body_ceiling = world.addStaticBox(0,GameManager::courtHeight()+.5,GameManager::courtWidth()+2,1);
    body_ceiling->SetUserData(NULL);
    body_left_wall = world.addStaticBox(GameManager::courtWidth()/2.+.5,GameManager::courtHeight()/2.,1,GameManager::courtHeight()+2.);
    body_left_wall->SetUserData(NULL);
    body_right_wall = world.addStaticBox(-GameManager::courtWidth()/2.-.5,GameManager::courtHeight()/2.,1,GameManager::courtHeight()+2.);
    body_right_wall->SetUserData(NULL);
    body_left_tack = world.addStaticBall(-GameManager::courtWidth()/2.,GameManager::courtHeight(),GameManager::tackRadius());
    body_left_tack->SetUserData(NULL);
    body_right_tack = world.addStaticBall(GameManager::courtWidth()/2.,GameManager::courtHeight(),GameManager::tackRadius());
    body_right_tack->SetUserData(NULL);
    body_net = world.addStaticBox(0,GameManager::netHeight()/2.,GameManager::netWidth(),GameManager::netHeight());
    body_net->SetUserData(NULL);

    body_ball = world.addBall(0,GameManager::ballReleaseHeight(),GameManager::ballRadius());
    ball = new Ball(body_ball);

    left_team = new Team(Team::LEFT);
    right_team = new Team(Team::RIGHT);

    body_left_player = world.addPlayer(-GameManager::courtWidth()/4.,0,GameManager::playerRadius());
    left_player = new Player(body_left_player,*left_team);
    body_right_player = world.addPlayer(GameManager::courtWidth()/4.,0,GameManager::playerRadius());
    right_player = new Player(body_right_player,*right_team);
}

void GameData::beginPoint(World& world) 
{
    Q_ASSERT_X(state==INIT || state==FINISHED,"game state","invalid state transition");

    // reset player position
    body_left_player->SetTransform(b2Vec2(-GameManager::courtWidth()/4.,0),0);
    body_right_player->SetTransform(b2Vec2(GameManager::courtWidth()/4.,0),0);
    body_left_player->SetLinearVelocity(b2Vec2(0,0));
    body_right_player->SetLinearVelocity(b2Vec2(0,0));

    // reset ball position
    body_ball->SetLinearVelocity(b2Vec2(0,0));
    body_ball->SetAngularVelocity(0);
    body_ball->SetAwake(false);

    // place ball in the good spot
    if (state==INIT) {
	Q_ASSERT(last_scoring_team==NULL);
	body_ball->SetTransform(b2Vec2(0,GameManager::ballReleaseHeight()),0); // place ball on net for first point
    } else {
	Q_ASSERT(last_scoring_team);
	switch (last_scoring_team->getField()) {
	    case Team::LEFT:
		body_ball->SetTransform(b2Vec2(-GameManager::courtWidth()/4.,GameManager::ballReleaseHeight()),0);
		break;
	    case Team::RIGHT:
		body_ball->SetTransform(b2Vec2(GameManager::courtWidth()/4.,GameManager::ballReleaseHeight()),0);
		break;
	};
    }

    last_transition_time = world.getTime();
    state = STARTING;
}

void GameData::stabilizePlayers(World* world)
{
    Q_UNUSED(world);

    left_player->checkJump(world->getTime());
    left_player->checkPosition(world->getTime());
    right_player->checkJump(world->getTime());
    right_player->checkPosition(world->getTime());
}

void GameData::checkState(World* world)
{
    if (state==STARTING) {
	if (world->getTime()-last_transition_time>GameManager::startingTime()) {
	    body_ball->SetAwake(true);
	    last_transition_time = world->getTime();
	    state = PLAYING;
	}
	return;
    }

    if (state==PLAYING) {
	for (const b2ContactEdge* ce = ball->getBody()->GetContactList(); ce; ce = ce->next) {
	    const b2Contact* contact = ce->contact;
	    if (!contact->IsTouching()) continue;

	    const b2Body* body1 = contact->GetFixtureA()->GetBody();
	    const b2Body* body2 = contact->GetFixtureB()->GetBody();

	    if(body1 == body_right_ground){
		last_scoring_team = left_team;
		last_scoring_team->teamScored();
		last_transition_time = world->getTime();
		state = FINISHED;
	    }

	    if(body1 == body_left_ground){
		last_scoring_team = right_team;
		last_scoring_team->teamScored();
		last_transition_time = world->getTime();
		state = FINISHED;
	    }
	}
	return;
    }
}
