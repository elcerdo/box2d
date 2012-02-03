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
    world->SetAutoClearForces(false);
    world->SetContinuousPhysics(true);
    world->SetSubStepping(false);
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

b2Body* World::addStaticBox(float x, float y, float width, float height)
{
    return addStaticBox(b2Vec2(x,y),width,height);
}

b2Body* World::addStaticBox(const b2Vec2 &pos, float width, float height)
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
    //fixtureDef.friction = 0;
    //fixtureDef.restitution = 0;

    b2Body* body = world->CreateBody(&bodyDef);
    body->CreateFixture(&fixtureDef);
    return body;
}

b2Body* World::addPlayer(float x, float y, float radius)
{
    return addPlayer(b2Vec2(x,y),radius);
}

b2Body* World::addPlayer(const b2Vec2 &pos, float radius)
{
    Q_ASSERT(world);

    b2BodyDef bodyDef;
    bodyDef.type = b2_kinematicBody;
    bodyDef.position = pos;

    b2PolygonShape shape;
    {
	const int nverts = 8;
	b2Vec2 verts[nverts];
	for (int kk=0; kk<nverts; kk++) {
	    float angle = b2_pi*kk/(nverts-1);
	    verts[kk] = b2Vec2(radius*cos(angle),radius*sin(angle));
	}
	shape.Set(verts,nverts);
    }

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1;
    //fixtureDef.friction = 1;
    //fixtureDef.restitution = 0;

    b2Body* body = world->CreateBody(&bodyDef);
    body->CreateFixture(&fixtureDef);
    return body;
}

b2Body* World::addBird(float x, float y, float radius)
{
    return addBird(b2Vec2(x,y),radius);
}

b2Body* World::addBird(const b2Vec2 &pos, float radius)
{
    Q_ASSERT(world);

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = pos;

    b2PolygonShape shape;
    {
	const int nverts = 3;
	b2Vec2 verts[nverts] = {b2Vec2(-radius,-radius),b2Vec2(radius,0),b2Vec2(-radius,radius)};
	shape.Set(verts,nverts);
    }

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1;
    //fixtureDef.friction = 0;
    //fixtureDef.restitution = 0;

    b2Body* body = world->CreateBody(&bodyDef);
    body->CreateFixture(&fixtureDef);
    body->SetGravityScale(0);
    return body;
}

b2Body* World::addStaticBall(float x, float y, float radius)
{
    return addStaticBall(b2Vec2(x,y),radius);
}

b2Body* World::addStaticBall(const b2Vec2 &pos, float radius)
{
    Q_ASSERT(world);

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position = pos;
    
    b2CircleShape shape;
    shape.m_radius = radius;

    b2Body* body = world->CreateBody(&bodyDef);
    body->CreateFixture(&shape,0);
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
    fixtureDef.friction = .1;
    fixtureDef.restitution = .9;

    b2Body* body = world->CreateBody(&bodyDef);
    body->SetLinearDamping(.1);
    body->SetAngularDamping(.05);
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
    emit preStepWorld(this);
    world->Step(dt/2.,6,2);
    emit postStepWorld(this);
    emit preStepWorld(this);
    world->Step(dt/2.,6,2);
    emit postStepWorld(this);
    emit worldStepped(this);
}
