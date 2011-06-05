#include "world.h"

#include <QDebug>

World::World(QObject *parent)
: QObject(parent), world(NULL), timer(NULL)
{
  timer = new QTimer(this);
  timer->setInterval(1000./60.);
  timer->setSingleShot(false);

  connect(timer,SIGNAL(timeout()),this,SLOT(stepWorld()));
}

World::~World()
{
  if (world) delete world;
}

void World::initialize(const b2Vec2 &gravity)
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

void World::setStepping(bool stepping)
{
  Q_ASSERT(world);
  if (stepping) timer->start();
  else timer->stop();
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
}

World::Bodies World::getBodies()
{
  Bodies bodies;
  for (b2Body* body=world->GetBodyList(); body!=NULL; body=body->GetNext())
    bodies.push_back(body);
  return bodies;
}

void World::stepWorld()
{
  qDebug() << "stepping";
  world->Step(1./60.,6,2);
  world->ClearForces();
  emit worldStepped();
}
