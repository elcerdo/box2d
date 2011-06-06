#ifndef __WORLD_H__
#define __WORLD_H__

#include <QObject>
#include <QTimer>
#include "Box2D/Box2D.h"

class World : public QObject {
Q_OBJECT
public:
  World(QObject *parent=NULL);
  ~World();

  void initialize(const b2Vec2 &gravity);
  b2Body* addGround();
  b2Body* addBox(float x, float y,float width=1,float height=1);
  b2Body* addBall(float x, float y);
  int getBodyCount() const;
  b2Body* getFirstBody();
  b2Joint* addDistanceJoint(b2Body* a, b2Body* b, const b2Vec2 &ca, const b2Vec2 &cb, bool collide=false);
  int getJointCount() const;
  b2Joint *getFirstJoint();
public slots:
  void setStepping(bool stepping);
protected slots:
  void stepWorld();
signals:
  void worldStepped(World*);
protected:
  b2World *world;
  QTimer *timer;
};

#endif

