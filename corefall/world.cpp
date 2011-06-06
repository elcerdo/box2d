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

b2Body* World::addGround()
{
  Q_ASSERT(world);

  b2BodyDef bodyDef;
  bodyDef.position.Set(0,-10);

  b2PolygonShape shape;
  shape.SetAsBox(50,10);

  b2Body* body = world->CreateBody(&bodyDef);
  body->CreateFixture(&shape,0);
  return body;
}

b2Joint* World::addDistanceJoint(b2Body* a, b2Body* b, const b2Vec2 &ca, const b2Vec2 &cb, bool collide)
{
  Q_ASSERT(world);

  b2DistanceJointDef jointDef;
  jointDef.Initialize(a,b,ca,cb);
  jointDef.collideConnected = collide;

  b2Joint *joint = world->CreateJoint(&jointDef);
  return joint;
}

void World::setStepping(bool stepping)
{
  Q_ASSERT(world);
  if (stepping) timer->start();
  else timer->stop();
}

int World::getBodyCount() const
{
  Q_ASSERT(world);
  return world->GetBodyCount();
}

int World::getJointCount() const
{
  Q_ASSERT(world);
  return world->GetJointCount();
}

b2Body* World::getFirstBody()
{
  Q_ASSERT(world);
  return world->GetBodyList();
}

b2Joint* World::getFirstJoint()
{
  Q_ASSERT(world);
  return world->GetJointList();
}

b2Body* World::addBox(float x, float y, float width, float height)
{
  Q_ASSERT(world);

  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(x,y);

  b2PolygonShape shape;
  shape.SetAsBox(width/2.,height/2.);

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &shape;
  fixtureDef.density = 1;
  fixtureDef.friction = .3;
  fixtureDef.restitution = .6;

  b2Body* body = world->CreateBody(&bodyDef);
  body->CreateFixture(&fixtureDef);
  return body;
}

b2Body* World::addBall(float x, float y)
{
  Q_ASSERT(world);

  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position.Set(x,y);

  b2CircleShape shape;
  shape.m_radius = 1;

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &shape;
  fixtureDef.density = 1;
  fixtureDef.friction = .3;
  fixtureDef.restitution = .6;

  b2Body* body = world->CreateBody(&bodyDef);
  body->CreateFixture(&fixtureDef);
  return body;
}

void World::stepWorld()
{
  qDebug() << "stepping";
  world->Step(1./60.,6,2);
  world->ClearForces();
  emit worldStepped(this);
}