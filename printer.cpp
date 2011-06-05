#include "printer.h"

#include <QDebug>
#include <QCoreApplication>


Printer::Printer(QObject *parent)
: QObject(parent), world(NULL), stepLeft(0)
{
  world = new World(this);
  world->initialize(b2Vec2(0,-10));
  world->addGround();
  world->addBall(0,1);
  world->addBall(.5,0);

  connect(world,SIGNAL(worldStepped()),this,SLOT(displayWorld()));
}

void Printer::start(int step) {
  stepLeft += step;
  world->setStepping(true);
}

void Printer::displayWorld() {
  World::Bodies bodies = world->getBodies();
  qDebug() << "found" << bodies.size() << "body(ies)";
  bool allSleep = true;
  for (World::Bodies::const_iterator ibody=bodies.begin(); ibody!=bodies.end(); ibody++) {
    const b2Body *body = *ibody;
    allSleep &= !body->IsAwake();
    qDebug() << "awake" << body->IsAwake() << "x" << body->GetPosition().x << "y" << body->GetPosition().y << "angle" << body->GetAngle()*180/M_PI;
    for (const b2Fixture* fixture=body->GetFixtureList(); fixture!=NULL; fixture=fixture->GetNext()) {
      qDebug() << "**" << "shapetype" << fixture->GetShape()->GetType();
    }
  }

  stepLeft--;
  if (stepLeft<=0  || allSleep) {
    emit done();
  }
}

