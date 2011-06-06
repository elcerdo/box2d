#ifndef __WORLD_H__
#define __WORLD_H__

#include <QObject>
#include <QTimer>
#include <QList>
#include <Box2D/Box2D.h>

class World : public QObject {
Q_OBJECT
public:
  typedef QList<b2Body*> Bodies;

  World(QObject *parent=NULL);
  ~World();

  void initialize(const b2Vec2 &gravity);
  void addGround();
  void addBall(float x, float y);
  Bodies getBodies();
public slots:
  void setStepping(bool stepping);
protected slots:
  void stepWorld();
signals:
  void worldStepped();
protected:
  b2World *world;
  QTimer *timer;
};

#endif

