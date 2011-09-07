#include "world.h"

#include <QDebug>

World::World(float dt, QObject *parent)
    : QObject(parent), world(NULL), timer(NULL), time(0), dt(dt)
{
    timer = new QTimer(this);
    timer->setInterval(1000.*dt);
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

b2Body* World::addGround(float x, float y, float width, float height)
{
    return addGround(b2Vec2(x,y),width,height);
}

b2Body* World::addGround(const b2Vec2 &pos, float width, float height)
{
    Q_ASSERT(world);

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position = pos;

    b2PolygonShape shape;
    shape.SetAsBox(width/2.,height/2.);

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

bool World::allBodiesAsleep() const
{
    bool allSleep = true;
    for (const b2Body* body=world->GetBodyList(); body!=NULL; body=body->GetNext()) {
        allSleep &= !body->IsAwake();
    }
    return allSleep;
}

void World::stepWorld()
{
    time += dt;
    world->Step(dt,6,2);
    world->ClearForces();
    emit worldStepped(this);
}
