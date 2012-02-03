#include "gamestate.h"

#include <QDebug>
#include <iostream>
#include "common.h"

GameState::GameState(World& world)
	: world(world)
{
    world.initialize(b2Vec2(0,0));

	{
		world.addStaticBox(boardSize/2.+boardBorderWidth/2.,0,boardBorderWidth,boardSize+2*boardBorderWidth);
		world.addStaticBox(-boardSize/2.-boardBorderWidth/2.,0,boardBorderWidth,boardSize+2*boardBorderWidth);
		world.addStaticBox(0,boardSize/2.+boardBorderWidth/2.,boardSize+2*boardBorderWidth,boardBorderWidth);
		world.addStaticBox(0,-boardSize/2.-boardBorderWidth/2.,boardSize+2*boardBorderWidth,boardBorderWidth);
	}

	addBall(0,0);
	for (int kk=0; kk<6; kk++)
	{
		const float angle = M_PI*kk/3.;
		addBall(2.1*ballRadius*cos(angle),2.1*ballRadius*sin(angle));
	}

	hitted = addBall(0.,.6);

}

void GameState::hitBall()
{
	hitted->SetLinearVelocity(b2Vec2(0.1,3));
}

b2Body* GameState::addBall(float x, float y)
{
	b2Body* ball = world.addBall(x,y,ballRadius);
	balls.push_back(ball);
	return ball;
}

void GameState::stabilizeBalls(World* world)
{
	for (Bodies::const_iterator iter=balls.begin(); iter!=balls.end(); iter++)
	{
		b2Body* ball = *iter;
		if (ball->GetLinearVelocity().Length() < .1)
		{
			ball->SetLinearVelocity(b2Vec2(0,0));
			ball->SetAngularVelocity(0);
		}
	}
}

