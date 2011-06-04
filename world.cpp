#include "world.h"

#include <QDebug>

World::World(QObject *parent)
: QObject(parent), gravity(0,-10), world(NULL)
{
}

World::~World()
{
  if (world) delete world;
}

void World::initialize()
{
  Q_ASSERT(world==NULL);
  world = new b2World(gravity,true);
}

void World::addGround()
{
  Q_ASSERT(world);

  b2BodyDef bodyDef;
  bodyDef.position.Set(0,-10);

  b2PolygonShape shape;
  shape.SetAsBox(10,10);

  b2Body* body = world->CreateBody(&bodyDef);
  body->CreateFixture(&shape,0);
}

void World::addBall(float x, float y)
{
  Q_ASSERT(world);

  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(x,y);

  b2PolygonShape shape;
  shape.SetAsBox(.5,.5);

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &shape;
  fixtureDef.density = 1;
  fixtureDef.friction = .3;

  b2Body* body = world->CreateBody(&bodyDef);
  body->CreateFixture(&fixtureDef);

  bodies.push_back(body);
}

void World::stepWorld()
{
  qDebug() << "stepping";
  world->Step(1./60.,6,2);
  world->ClearForces();
  emit worldStepped();
}
