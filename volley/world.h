#ifndef __WORLD_H__
#define __WORLD_H__

#include <QObject>
#include <QTimer>
#include <Box2D/Box2D.h>

class World : public QObject {
Q_OBJECT
public:
  World(float dt, QObject *parent=NULL);
  ~World();

  void initialize(const b2Vec2 &gravity);
  b2Body* addGround();
  b2Body* addBox(const b2Vec2 &pos, float width=1, float height=1);
  b2Body* addBox(float x, float y, float width=1, float height=1);
  b2Body* addBall(const b2Vec2 &pos, float radius);
  b2Body* addBall(float x, float y, float radius=1);
  bool allBodiesAsleep() const;
  int getBodyCount() const;
  b2Body* getFirstBody();

  b2Joint* addDistanceJoint(b2Body* a, b2Body* b, const b2Vec2 &ca, const b2Vec2 &cb, bool collide=false);
  b2Joint* addHingeJoint(b2Body* a,b2Body *b, const b2Vec2 &pos, bool collide=false, float torque=-1, float speed=0);
  void destroyJoint(b2Joint* joint);
  int getJointCount() const;
  b2Joint *getFirstJoint();

  void resetTime();
  float getTime() const;
protected:
public slots:
  void setStepping(bool stepping);
  void stepWorld();
signals:
  void worldStepped(World*);
protected:
  b2World *world;
  QTimer *timer;
  float time;
  float dt;
};

#endif

