#include "world.h"

#include <QDebug>

World::World(QObject *parent)
: QObject(parent), world(NULL), timer(NULL), time(0)
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

void World::resetTime()
{
  time = 0;
}

float World::getTime() const
{
  return time;
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

b2Joint* World::addHingeJoint(b2Body* a,b2Body *b, const b2Vec2 &pos, bool collide, float torque, float speed)
{
  Q_ASSERT(world);

  b2RevoluteJointDef jointDef;
  jointDef.Initialize(a,b,pos);
  jointDef.enableLimit = false;
  jointDef.enableMotor = false;
  jointDef.collideConnected = collide;

  if (torque>=0) {
    jointDef.enableMotor = true;
    jointDef.maxMotorTorque = torque;
    jointDef.motorSpeed = speed;
  }

  b2Joint *joint = world->CreateJoint(&jointDef);
  return joint;
}

void World::destroyJoint(b2Joint* joint)
{
  Q_ASSERT(world);
  world->DestroyJoint(joint);
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
  return addBox(b2Vec2(x,y),width,height);
}

b2Body* World::addBox(const b2Vec2 &pos, float width, float height)
{
  Q_ASSERT(world);

  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position = pos;

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

b2Body* World::addBall(float x, float y, float radius)
{
  return addBall(b2Vec2(x,y),radius);
}

b2Body* World::addBall(const b2Vec2 &pos, float radius)
{
  Q_ASSERT(world);

  b2BodyDef bodyDef;
  bodyDef.type = b2_dynamicBody;
  bodyDef.position = pos;

  b2CircleShape shape;
  shape.m_radius = radius;

  b2FixtureDef fixtureDef;
  fixtureDef.shape = &shape;
  fixtureDef.density = 1;
  fixtureDef.friction = .3;
  fixtureDef.restitution = .6;

  b2Body* body = world->CreateBody(&bodyDef);
  body->CreateFixture(&fixtureDef);
  return body;
}

const float motorRadius = 1.5;
const float mainLength  = 20.;
const float mainHeight  = 2;
const float upperExtension = 3;
const float legWidth = 3;
const float legHeight = 5;
const float footHeight = 4;
const int   nLegs = 4;

void World::buildLegPair(const b2Vec2 &center, b2Body* main, b2Body* motor,int category)
{
  Q_ASSERT(world);

  { // left leg
    b2Vec2 baseVec = center+b2Vec2(-mainLength/2.,0);

    b2Body* upperPart = NULL;
    {
      b2BodyDef bodyDef;
      bodyDef.type = b2_dynamicBody;
      bodyDef.position = baseVec;

      b2Vec2 points[] = {b2Vec2(0,0),b2Vec2(0,motorRadius+upperExtension),b2Vec2(-legWidth,0)};
      b2PolygonShape shape;
      shape.Set(points,3);

      b2FixtureDef fixtureDef;
      fixtureDef.shape = &shape;
      fixtureDef.density = 1;
      fixtureDef.friction = 0;
      fixtureDef.restitution = 0;
      fixtureDef.filter.categoryBits = 1 << (category+1);
      fixtureDef.filter.maskBits = 1;

      b2Body* body = world->CreateBody(&bodyDef);
      body->CreateFixture(&fixtureDef);

      upperPart = body;
    }
    this->addHingeJoint(upperPart,main,baseVec);
    this->addDistanceJoint(motor,upperPart,motor->GetWorldCenter()+b2Vec2(0,motorRadius),baseVec + b2Vec2(0,motorRadius+upperExtension));

    b2Body* lowerPart = NULL;
    {
      b2BodyDef bodyDef;
      bodyDef.type = b2_dynamicBody;
      bodyDef.position = baseVec+b2Vec2(0,-legHeight);

      b2Vec2 points[] = {b2Vec2(0,0),b2Vec2(-legWidth,0),b2Vec2(0,-footHeight)};
      b2PolygonShape shape;
      shape.Set(points,3);

      b2FixtureDef fixtureDef;
      fixtureDef.shape = &shape;
      fixtureDef.density = 1;
      fixtureDef.friction = 1;
      fixtureDef.restitution = 0;
      fixtureDef.filter.categoryBits = 1 << (category+1);
      fixtureDef.filter.maskBits = 1;

      b2Body* body = world->CreateBody(&bodyDef);
      body->CreateFixture(&fixtureDef);

      lowerPart = body;
    }
    this->addDistanceJoint(upperPart,lowerPart,baseVec                      ,baseVec + b2Vec2(0,-legHeight)        );
    this->addDistanceJoint(upperPart,lowerPart,baseVec + b2Vec2(-legWidth,0),baseVec + b2Vec2(-legWidth,-legHeight));
    this->addDistanceJoint(motor,lowerPart,motor->GetWorldCenter()+b2Vec2(0,motorRadius),baseVec + b2Vec2(0,-legHeight));
  }

  //this->rotateEngine(engine,b2_pi);

  { // right leg
    b2Vec2 baseVec = center+b2Vec2(mainLength/2.,0);

    b2Body* upperPart = NULL;
    {
      b2BodyDef bodyDef;
      bodyDef.type = b2_dynamicBody;
      bodyDef.position = baseVec;

      b2Vec2 points[] = {b2Vec2(0,0),b2Vec2(legWidth,0),b2Vec2(0,motorRadius+upperExtension)};
      b2PolygonShape shape;
      shape.Set(points,3);

      b2FixtureDef fixtureDef;
      fixtureDef.shape = &shape;
      fixtureDef.density = 1;
      fixtureDef.friction = 0;
      fixtureDef.restitution = 0;
      fixtureDef.filter.categoryBits = 1 << (category+1);
      fixtureDef.filter.maskBits = 1;

      b2Body* body = world->CreateBody(&bodyDef);
      body->CreateFixture(&fixtureDef);

      upperPart = body;
    }
    this->addHingeJoint(upperPart,main,baseVec);
    this->addDistanceJoint(motor,upperPart,motor->GetWorldCenter()+b2Vec2(0,motorRadius),baseVec + b2Vec2(0,motorRadius+upperExtension),true);

    b2Body* lowerPart = NULL;
    {
      b2BodyDef bodyDef;
      bodyDef.type = b2_dynamicBody;
      bodyDef.position = baseVec+b2Vec2(0,-legHeight);

      b2Vec2 points[] = {b2Vec2(0,0),b2Vec2(0,-footHeight),b2Vec2(legWidth,0)};
      b2PolygonShape shape;
      shape.Set(points,3);

      b2FixtureDef fixtureDef;
      fixtureDef.shape = &shape;
      fixtureDef.density = 1;
      fixtureDef.friction = 1;
      fixtureDef.restitution = 0;
      fixtureDef.filter.categoryBits = 1 << (category+1);
      fixtureDef.filter.maskBits = 1;

      b2Body* body = world->CreateBody(&bodyDef);
      body->CreateFixture(&fixtureDef);

      lowerPart = body;
    }
    this->addDistanceJoint(upperPart,lowerPart,baseVec                     ,baseVec + b2Vec2(0,-legHeight)        ,true);
    this->addDistanceJoint(upperPart,lowerPart,baseVec + b2Vec2(legWidth,0),baseVec + b2Vec2(legWidth,-legHeight),true);
    this->addDistanceJoint(motor,lowerPart,motor->GetWorldCenter()+b2Vec2(0,motorRadius),baseVec + b2Vec2(0,-legHeight),true);
  }
}

b2Body* World::buildRobot(const b2Vec2 &base, b2Body* ground)
{
  Q_ASSERT(world);

  const b2Vec2 center = base+b2Vec2(0,legHeight+footHeight);

  b2Body* main = NULL;
  {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = center;

    b2Vec2 points[] = {b2Vec2(mainLength/2.,0),b2Vec2(0,mainHeight/2.),b2Vec2(-mainLength/2.,0),b2Vec2(0,-mainHeight/2.)};
    b2PolygonShape shape;
    shape.Set(points,4);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1;
    fixtureDef.friction = 0;
    fixtureDef.restitution = 0;

    b2Body* body = world->CreateBody(&bodyDef);
    body->CreateFixture(&fixtureDef);

    main = body;
  }
  b2Joint* fix0 = this->addHingeJoint(main,ground,center-b2Vec2(mainLength/3.,0));
  b2Joint* fix1 = this->addHingeJoint(main,ground,center+b2Vec2(mainLength/3.,0));

  b2Body* motor = this->addBall(center,motorRadius);
  b2RevoluteJoint* engine = static_cast<b2RevoluteJoint*>(this->addHingeJoint(motor,main,motor->GetWorldCenter(),false,10000,0));

  for (int kk=0; kk<nLegs; kk++) {
    rotateEngine(engine,kk*2*b2_pi/nLegs);
    buildLegPair(center,main,motor,kk);
  }

  this->destroyJoint(fix0);
  this->destroyJoint(fix1);
  engine->SetMotorSpeed(b2_pi/2.);
  return main;
}

void World::rotateEngine(b2RevoluteJoint *engine, float angle, float tol)
{
  Q_ASSERT(world);

  const float tau = 2;
  const float startTime = this->getTime();
  while (true) {
    float error = engine->GetJointAngle() - angle;
    if (fabs(error)<tol) break;
    engine->SetMotorSpeed(-error/tau);
    this->stepWorld();
  }
  const float endTime = this->getTime();

  //qDebug() << "angle" << engine->GetJointAngle()*180/b2_pi << "time" << (endTime-startTime);
}


void World::stepWorld()
{
  static const float dt = 1./60.;
  time += dt;
  world->Step(dt,6,2);
  world->ClearForces();
  emit worldStepped(this);
}
