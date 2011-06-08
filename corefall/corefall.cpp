#include <QApplication>
#include "printer.h"
#include "drawer.h"
#include "world.h"
#include "pickling/chooseser.h"

void test()
{
  Tab dict;
  dict["patate"] = 42.;
  dict["coco"] = "pierre";
  
  DumpValToFile(dict,"patate.pck");
}

int main(int argc,char * argv[])
{
  test();

  QApplication app(argc,argv);

  World world;
  world.initialize(b2Vec2(0,-10));
  b2Body* ground = world.addGround();
  //b2Body* poutre = world.addBox(-15,8,10,.2);
  //world.addDistanceJoint(poutre,ground,b2Vec2(-19.9,8),b2Vec2(-19.9,-0.5),true);
  //world.addDistanceJoint(poutre,ground,b2Vec2(-10.1,8),b2Vec2(-10.1,-0.5),true);
  //world.addBox(-10,10);
  //world.addBox(3,10);
  //world.addBox(-1,7);
  //world.addBall(0,5);

  Robot robot = world.addRobot(b2Vec2(-30,0),ground);
  RobotTimer robotTimer(robot);
  robotTimer.setRange(-20,20);

  //Printer printer;
  Drawer drawer;
  drawer.resize(1000,500);
  drawer.show();

  world.setStepping(true);

  QObject::connect(&world,SIGNAL(worldStepped(World*)),&drawer,SLOT(displayWorld(World*)));
  QObject::connect(&world,SIGNAL(worldStepped(World*)),&robotTimer,SLOT(analyseWorld(World*)));
  //QObject::connect(&world,SIGNAL(worldStepped(World*)),&printer,SLOT(displayWorld(World*)));
  //QObject::connect(&printer,SIGNAL(done()),&app,SLOT(quit()));

  return app.exec();
}

