#ifndef __WORLD_H__
#define __WORLD_H__

#include <QObject>
#include <QList>
#include <Box2D/Box2D.h>

class World : public QObject {
Q_OBJECT
public:
  typedef QList<b2Body*> Bodies;

  World(QObject *parent=NULL);
  ~World();

  b2Vec2 gravity;
  void initialize();
  Bodies bodies;
  void addGround();
  void addBall(float x, float y);

public slots:
  void stepWorld();
signals:
  void worldStepped();
protected:
  b2World *world;
};

#endif

