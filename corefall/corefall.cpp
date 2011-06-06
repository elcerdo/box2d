#include <QCoreApplication>
#include "printer.h"
#include "world.h"

int main(int argc,char * argv[])
{
  QCoreApplication app(argc,argv);

  World world;
  world.initialize(b2Vec2(0,-10));
  world.addGround();
  world.addBox(0,1);
  world.addBox(.5,0);
  world.addBall(0,2);

  Printer printer;

  world.setStepping(true);

  QObject::connect(&world,SIGNAL(worldStepped(World*)),&printer,SLOT(displayWorld(World*)));
  QObject::connect(&printer,SIGNAL(done()),&app,SLOT(quit()));

  return app.exec();
}

