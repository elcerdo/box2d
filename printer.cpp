#include "printer.h"

#include <QDebug>
#include <QCoreApplication>


Printer::Printer(QObject *parent)
: QObject(parent), world(NULL), timer(NULL), stepLeft(0)
{
  world = new World(this);
  world->initialize();
  world->addGround();
  world->addBall();

  timer = new QTimer(this);
  timer->setInterval(1000./60.);
  timer->setSingleShot(false);

  connect(timer,SIGNAL(timeout()),this,SLOT(bang()));
  connect(timer,SIGNAL(timeout()),world,SLOT(stepWorld()));
  connect(world,SIGNAL(worldStepped()),this,SLOT(displayWorld()));
}

void Printer::start(int step) {
  stepLeft += step;
  timer->start();
}

void Printer::bang() {
  qDebug() << "timer triggered" << stepLeft;

  stepLeft--;
  if (stepLeft<=0) {
    timer->stop();
  }
}

void Printer::displayWorld() {
  qDebug() << "found" << world->bodies.size() << "body(ies)";
  bool allSleep = true;
  for (World::Bodies::const_iterator ibody=world->bodies.begin(); ibody!=world->bodies.end(); ibody++) {
    const b2Body *body = *ibody;
    allSleep &= !body->IsAwake();
    qDebug() << "awake" << body->IsAwake() << "x" << body->GetPosition().x << "y" << body->GetPosition().y << "angle" << body->GetAngle();
  }
  if (stepLeft<=0  || allSleep) {
    emit done();
  }
}

