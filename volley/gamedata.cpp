#include "gamedata.h"

#include <QDebug>

static const float scene_height = 15;
static const float court_width = 20;
static const float court_height = 30;
static const float court_net_height = 3;
static const float court_net_width = 0.2;
static const float player_radius = 1.2;
static const float player_speed = 8;
static const float ball_radius = .5;
static const float tack_radius = .3;

static const float max_jump_height = 2.;
static const float gravity = 10;
static const float jump_factor = 3.;
static const float basic_jump_speed = 5.;

GameData::GameData(World& world, QObject* parent) 
    : QObject(parent)
{
    buildCourt(world);
}

float GameData::courtWidth() { return court_width; }
float GameData::courtHeight() { return court_height; }
float GameData::netHeight() { return court_net_height; }
float GameData::netWidth() { return court_net_width; }
float GameData::ballRadius() { return ball_radius; }
float GameData::playerRadius() { return player_radius; }
float GameData::sceneHeight() { return scene_height; }

const b2Body* GameData::getBall() const { return ball; }
const b2Body* GameData::getLeftPlayer() const { return left_player; }
const b2Body* GameData::getRightPlayer() const { return right_player; }

void GameData::buildCourt(World &world)
{
    left_ground = world.addStaticBox(-court_width/4.-.5,-.5,court_width/2.+1,1);
    right_ground = world.addStaticBox(court_width/4.+.5,-.5,court_width/2.+1,1);
    ceiling = world.addStaticBox(0,court_height+.5,court_width+2,1);
    left_wall = world.addStaticBox(court_width/2.+.5,court_height/2.,1,court_height+2.);
    right_wall = world.addStaticBox(-court_width/2.-.5,court_height/2.,1,court_height+2.);
    left_tack = world.addStaticBall(-court_width/2.-tack_radius/2.,scene_height+2,tack_radius);
    right_tack = world.addStaticBall(court_width/2.+tack_radius/2.,scene_height+2,tack_radius);
    net = world.addStaticBox(0,court_net_height/2.,court_net_width,court_net_height);

    ball = world.addBall(0,4,ball_radius);

    left_player = world.addPlayer(-court_width/4.,0,player_radius);
    right_player = world.addPlayer(court_width/4.,0,player_radius);
    score_left_player = 0;
    score_right_player = 0;
    right_player_jumping = false;
    left_player_jumping = false;
    // right_player_jump_speed = 5.;
    // left_player_jump_speed = 5.;
    current_state = STARTPOINT;
}

void GameData::beginPoint()
{
 
  //  if(current_state == STARTPOINT){
    
  //}
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
    ball->SetAwake(true);
}

void GameData::leftPlayerGoLeft()
{
    left_player->SetLinearVelocity(b2Vec2(-player_speed,left_player->GetLinearVelocity().y));
}

void GameData::leftPlayerGoRight()
{
    left_player->SetLinearVelocity(b2Vec2(player_speed,left_player->GetLinearVelocity().y));
}

void GameData::leftPlayerStopLeft()
{
    if (left_player->GetLinearVelocity().x<0) left_player->SetLinearVelocity(b2Vec2(0,left_player->GetLinearVelocity().y));
}

void GameData::leftPlayerStopRight()
{
    if (left_player->GetLinearVelocity().x>0) left_player->SetLinearVelocity(b2Vec2(0,left_player->GetLinearVelocity().y));
}

void GameData::leftPlayerJump(float time)
{
  if(!left_player_jumping){
    left_player_jumping = true;
    left_player_jump_time = time;
    left_player_jump_speed = basic_jump_speed;
    left_player->SetLinearVelocity(b2Vec2(left_player->GetLinearVelocity().x, jump_factor*left_player_jump_speed));
  }
  //left_player_jump_speed = 10.;
}

void GameData::leftPlayerStopJump(float time)
{
  if(left_player_jump_speed - gravity*(time-left_player_jump_time) > 0)
    left_player_jump_speed = gravity*(time-left_player_jump_time);
  //left_player_jumping = false;
  // left_player_jump_speed = 0.;
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
    ball->SetAwake(true);
}

void GameData::rightPlayerGoLeft()
{
  right_player->SetLinearVelocity(b2Vec2(-player_speed, right_player->GetLinearVelocity().y));
}

void GameData::rightPlayerGoRight()
{
    right_player->SetLinearVelocity(b2Vec2(player_speed,right_player->GetLinearVelocity().y));
}

void GameData::rightPlayerStopLeft()
{
    if (right_player->GetLinearVelocity().x<0) right_player->SetLinearVelocity(b2Vec2(0,right_player->GetLinearVelocity().y));
}


void GameData::rightPlayerStopRight()
{
    if (right_player->GetLinearVelocity().x>0) right_player->SetLinearVelocity(b2Vec2(0,right_player->GetLinearVelocity().y));
}

void GameData::rightPlayerJump(float time)
{
  if(!right_player_jumping){
    right_player_jump_speed = basic_jump_speed;
    right_player_jumping = true;
    right_player_jump_time = time;
    right_player->SetLinearVelocity(b2Vec2(right_player->GetLinearVelocity().x, jump_factor*right_player_jump_speed));
  }
   
}

void GameData::rightPlayerStopJump(float time)
{
  if(right_player_jump_speed - gravity*(time-right_player_jump_time) > 0)
    right_player_jump_speed = gravity*(time-right_player_jump_time);
  //right_player_jumping = false;
}

int GameData::leftPlayerScore() const
{
    return score_left_player;
}

int GameData::rightPlayerScore() const
{
    return score_right_player;
}

void GameData::stabilizePlayers(World* world)
{
    Q_UNUSED(world);

    if(right_player_jumping){
    //   if(right_player->GetPosition().y < max_jump_height){
    // 	right_player->SetLinearVelocity(b2Vec2(right_player->GetLinearVelocity().x, right_player->GetLinearVelocity().y+1.));
    //   }
    //   else
    // 	right_player_jumping = false;
    // }
    // else{
      if(right_player->GetPosition().y < .05){
	right_player->SetTransform(b2Vec2(right_player->GetPosition().x, 0),0);
	right_player->SetLinearVelocity(b2Vec2(right_player->GetLinearVelocity().x, 0));
	right_player_jumping = false;
      }
      else{
	right_player->SetLinearVelocity(b2Vec2(right_player->GetLinearVelocity().x, jump_factor*(right_player_jump_speed - gravity*(world->getTime()-right_player_jump_time))));
      }
    }
    
    if(left_player_jumping){
    //   if(left_player->GetPosition().y < max_jump_height){
    //  	left_player->SetLinearVelocity(b2Vec2(left_player->GetLinearVelocity().x, left_player_jump_speed - 9.81*(world->getTime()-left_player_jump_time)));
    // 	qDebug() << "time : " << world->getTime()-left_player_jump_time;
    //   }
    //   else
    //  	leftPlayerStopJump();
    // }
    // else{
      if(left_player->GetPosition().y < .05){
	left_player->SetTransform(b2Vec2(left_player->GetPosition().x, 0),0);
	left_player->SetLinearVelocity(b2Vec2(left_player->GetLinearVelocity().x, 0));
	left_player_jumping = false;
      }
      else{
	left_player->SetLinearVelocity(b2Vec2(left_player->GetLinearVelocity().x, jump_factor*(left_player_jump_speed - gravity*(world->getTime()-left_player_jump_time))));
      }
    }
    
    
    { // check left player position
	const float left_player_x = left_player->GetPosition().x;
	const float left_player_y = left_player->GetPosition().y;
	const float left_player_y_velocity = left_player->GetLinearVelocity().y;
	static const float left_player_x_min = -court_width/2.+player_radius;
	static const float left_player_x_max = -court_net_width/2.-player_radius;
	if (left_player_x<left_player_x_min) {
	    left_player->SetTransform(b2Vec2(left_player_x_min,left_player_y),0);
	    left_player->SetLinearVelocity(b2Vec2(0,left_player_y_velocity));
	}
	if (left_player_x>left_player_x_max) {
	    left_player->SetTransform(b2Vec2(left_player_x_max,left_player_y),0);
	    left_player->SetLinearVelocity(b2Vec2(0,left_player_y_velocity));
	}
    }

    { // check right player position
	const float right_player_x = right_player->GetPosition().x;
	const float right_player_y = right_player->GetPosition().y;
	const float right_player_y_velocity = right_player->GetLinearVelocity().y;
	static const float right_player_x_max = court_width/2.-player_radius;
	static const float right_player_x_min = court_net_width/2.+player_radius;
	if (right_player_x<right_player_x_min) {
	    right_player->SetTransform(b2Vec2(right_player_x_min,right_player_y),0);
	    right_player->SetLinearVelocity(b2Vec2(0,right_player_y_velocity));
	}
	if (right_player_x>right_player_x_max) {
	    right_player->SetTransform(b2Vec2(right_player_x_max,right_player_y),0);
	    right_player->SetLinearVelocity(b2Vec2(0,right_player_y_velocity));
	}
    }

}

void GameData::checkPoints(World* world)
{
    Q_UNUSED(world);
    //if(current_state == PLAYING){
      for(b2ContactEdge* ce = ball->GetContactList(); ce; ce = ce->next){
	b2Contact* contact = ce->contact;
	if(contact->IsTouching()){
	    const b2Body* body1 = contact->GetFixtureA()->GetBody();
	    const b2Body* body2 = contact->GetFixtureB()->GetBody();
	    if(body1 == right_ground){
	      score_left_player++;
	      leftPlayerStart();
	    }
	    if(body1 == left_ground){
	      score_right_player++;
	      rightPlayerStart();
	    }
	  }
	  }
      //}
    
}
