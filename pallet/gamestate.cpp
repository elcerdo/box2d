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

	palet = world.addBall(0,0,ballRadius);
	palet->SetLinearVelocity(b2Vec2(1,2));

}

void GameState::stabilizeBalls(World* world)
{
	if (palet->GetLinearVelocity().Length() < .1)
	{
		palet->SetLinearVelocity(b2Vec2(0,0));
		palet->SetAngularVelocity(0);
	}
}


