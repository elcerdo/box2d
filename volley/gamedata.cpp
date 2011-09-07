#include "gamedata.h"

#include <QDebug>

static const float court_width = 20;
static const float court_height = 20;
static const float court_net_height = 1;
static const float court_net_width = 0.05;
static const float player_radius = 1.2;
static const float player_speed = 8;

GameData::GameData(World& world, QObject* parent) 
    : QObject(parent)
{
    buildCourt(world);
}

void GameData::buildCourt(World &world)
{
    left_ground = world.addGround(-court_width/4.-.5,-.5,court_width/2.+1,1);
    right_ground = world.addGround(court_width/4.+.5,-.5,court_width/2.+1,1);
    ceiling = world.addGround(0,court_height+.5,court_width+2,1);
    left_wall = world.addGround(court_width/2.+.5,court_height/2.,1,court_height+2.);
    right_wall = world.addGround(-court_width/2.-.5,court_height/2.,1,court_height+2.);
    net = world.addGround(0,court_net_height/2.,court_net_width,court_net_height);

    ball = world.addBall(0,4,.1);

    left_player = world.addPlayer(-court_width/4.,0,player_radius);
    right_player = world.addPlayer(court_width/4.,0,player_radius);
    score_left_player = 0;
    score_right_player = 0;

}

// left player stuff
void GameData::leftPlayerStart()
{
    qDebug() << "left player start";

    left_player->SetTransform(b2Vec2(-court_width/4.,0),0);
    right_player->SetTransform(b2Vec2(court_width/4.,0),0);
    left_player->SetLinearVelocity(b2Vec2(0,0));
    right_player->SetLinearVelocity(b2Vec2(0,0));

    ball->SetTransform(b2Vec2(-court_width/4.,5),0);
    ball->SetLinearVelocity(b2Vec2(0,0));
    ball->SetAngularVelocity(0);
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

// right player stuff
void GameData::rightPlayerStart()
{
    qDebug() << "right player start";

    left_player->SetTransform(b2Vec2(-court_width/4.,0),0);
    right_player->SetTransform(b2Vec2(court_width/4.,0),0);
    left_player->SetLinearVelocity(b2Vec2(0,0));
    right_player->SetLinearVelocity(b2Vec2(0,0));

    ball->SetTransform(b2Vec2(court_width/4.,5),0);
    ball->SetLinearVelocity(b2Vec2(0,0));
    ball->SetAngularVelocity(0);
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

void GameData::stabilizePlayers(World* world)
{
    Q_UNUSED(world);

    { // check left player position
	const float left_player_x = left_player->GetPosition().x;
	static const float left_player_x_min = -court_width/2.+player_radius;
	static const float left_player_x_max = -court_net_width/2.-player_radius;
	if (left_player_x<left_player_x_min) {
	    left_player->SetTransform(b2Vec2(left_player_x_min,0),0);
	    left_player->SetLinearVelocity(b2Vec2(0,0));
	}
	if (left_player_x>left_player_x_max) {
	    left_player->SetTransform(b2Vec2(left_player_x_max,0),0);
	    left_player->SetLinearVelocity(b2Vec2(0,0));
	}
    }

    { // check right player position
	const float right_player_x = right_player->GetPosition().x;
	static const float right_player_x_max = court_width/2.-player_radius;
	static const float right_player_x_min = court_net_width/2.+player_radius;
	if (right_player_x<right_player_x_min) {
	    right_player->SetTransform(b2Vec2(right_player_x_min,0),0);
	    right_player->SetLinearVelocity(b2Vec2(0,0));
	}
	if (right_player_x>right_player_x_max) {
	    right_player->SetTransform(b2Vec2(right_player_x_max,0),0);
	    right_player->SetLinearVelocity(b2Vec2(0,0));
	}
    }

}

void GameData::pointMarked()
{
  for(b2ContactEdge* ce = ball->GetContactList(); ce; ce = ce->next){
    b2Contact* contact = ce->contact;
    const b2Body* body1 = contact->GetFixtureA()->GetBody();
    const b2Body* body2 = contact->GetFixtureB()->GetBody();
    if(body1 == right_ground){
      qDebug() << "Point player 1";
      score_right_player++;
      
    }
    if(body1 == left_ground){
      qDebug() << "Point player 2";
      score_left_player++;
    }
  }
}
