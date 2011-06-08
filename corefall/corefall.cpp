#include <QApplication>
#include "drawer.h"
#include "world.h"
#include "logic.h"
#include "robot.h"

int main(int argc,char * argv[])
{
  if (argc<2) exit(2);
  RobotDef robotDef;
  robotDef.loadFromFile(argv[1]);
  robotDef.print();

  QApplication app(argc,argv);

  World world;
  world.initialize(b2Vec2(0,-10));
  b2Body* ground = world.addGround();

  Robot robot = world.addRobot(b2Vec2(-30,0),robotDef,ground);
  RobotTimer robotTimer(robot);
  robotTimer.setRange(-20,20);

  Drawer drawer;
  drawer.resize(1000,500);
  drawer.show();

  Logic logic(robot);

  QObject::connect(&world,SIGNAL(worldStepped(World*)),&drawer,SLOT(displayWorld(World*)));
  QObject::connect(&world,SIGNAL(worldStepped(World*)),&robotTimer,SLOT(analyseWorld(World*)));
  QObject::connect(&world,SIGNAL(worldStepped(World*)),&logic,SLOT(analyseWorld(World*)));
  QObject::connect(&robotTimer,SIGNAL(done()),&app,SLOT(quit()));

  world.setStepping(true);

  return app.exec();
}

