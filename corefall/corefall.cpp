#include <QApplication>
#include "printer.h"
#include "drawer.h"
#include "world.h"

int main(int argc,char * argv[])
{
  QApplication app(argc,argv);

  const float motorRadius = 1.;
  World world;
  {
    world.initialize(b2Vec2(0,-10));
    b2Body* ground = world.addGround();
    b2Body* poutre = world.addBox(-15,8,10,.2);
    world.addDistanceJoint(poutre,ground,b2Vec2(-19.9,8),b2Vec2(-19.9,-0.5),true);
    world.addDistanceJoint(poutre,ground,b2Vec2(-10.1,8),b2Vec2(-10.1,-0.5),true);
    b2Body* upperPart = world.addBox(18,22,4,4);
    world.addHingeJoint(upperPart,ground,b2Vec2(20,20));
    b2Body* lowerPart = world.addBox(18,14,4,4);
    world.addDistanceJoint(upperPart,lowerPart,b2Vec2(16,20),b2Vec2(16,16),true);
    world.addDistanceJoint(upperPart,lowerPart,b2Vec2(20,20),b2Vec2(20,16),true);
    b2Body* motor = world.addBall(28,20,motorRadius);
    world.addHingeJoint(motor,ground,motor->GetWorldCenter(),false,10000000,2*b2_pi);
    world.addDistanceJoint(motor,upperPart,motor->GetWorldCenter()+b2Vec2(motorRadius,0),upperPart->GetWorldCenter()+b2Vec2(2,2));
    world.addDistanceJoint(motor,lowerPart,motor->GetWorldCenter()+b2Vec2(motorRadius,0),lowerPart->GetWorldCenter()+b2Vec2(2,2));
    world.addBox(-10,10);
    world.addBox(3,10);
    world.addBox(-1,7);
    world.addBall(0,5);
  }


  //Printer printer;
  Drawer drawer;
  drawer.resize(500,500);
  drawer.show();

  world.setStepping(true);

  QObject::connect(&world,SIGNAL(worldStepped(World*)),&drawer,SLOT(displayWorld(World*)));
  //QObject::connect(&world,SIGNAL(worldStepped(World*)),&printer,SLOT(displayWorld(World*)));
  //QObject::connect(&printer,SIGNAL(done()),&app,SLOT(quit()));

  return app.exec();
}

