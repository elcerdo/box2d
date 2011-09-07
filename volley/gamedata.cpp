#include "gamedata.h"

#include <iostream>
using std::endl;
using std::cout;

static const float court_width = 20;
static const float court_height = 20;
static const float court_net_height = 1;
static const float player_radius = 1.5;
static const float player_speed = 8;

GameData::GameData(World& world) 
{
    buildCourt(world);
}

void GameData::buildCourt(World &world)
{
    cout << "building court" << endl;
    left_ground = world.addGround(-court_width/4.-.5,-.5,court_width/2.+1,1);
    right_ground = world.addGround(court_width/4.+.5,-.5,court_width/2.+1,1);
    ceiling = world.addGround(0,court_height+.5,court_width+2,1);
    left_wall = world.addGround(court_width/2.+.5,court_height/2.,1,court_height+2.);
    right_wall = world.addGround(-court_width/2.-.5,court_height/2.,1,court_height+2.);
    net = world.addGround(0,court_net_height/2.,0.05,court_net_height);

    ball = world.addBall(0,4,.1);

    left_player = world.addPlayer(-court_width/4.,0,player_radius);
    right_player = world.addPlayer(court_width/4.,0,player_radius);
}

void GameData::leftPlayerGoLeft()
{
    left_player->SetLinearVelocity(b2Vec2(-player_speed,0));
}

void GameData::leftPlayerGoRight()
{
    left_player->SetLinearVelocity(b2Vec2(player_speed,0));
}

void GameData::leftPlayerStopLeft()
{
    if (left_player->GetLinearVelocity().x<0) left_player->SetLinearVelocity(b2Vec2(0,0));
}

void GameData::leftPlayerStopRight()
{
    if (left_player->GetLinearVelocity().x>0) left_player->SetLinearVelocity(b2Vec2(0,0));
}

void GameData::rightPlayerGoLeft()
{
    right_player->SetLinearVelocity(b2Vec2(-player_speed,0));
}

void GameData::rightPlayerGoRight()
{
    right_player->SetLinearVelocity(b2Vec2(player_speed,0));
}

void GameData::rightPlayerStopLeft()
{
    if (right_player->GetLinearVelocity().x<0) right_player->SetLinearVelocity(b2Vec2(0,0));
}

void GameData::rightPlayerStopRight()
{
    if (right_player->GetLinearVelocity().x>0) right_player->SetLinearVelocity(b2Vec2(0,0));
}
