#include <QApplication>
#include "printer.h"
#include "drawer.h"
#include "world.h"

void rotateEngine(World &world, b2RevoluteJoint *engine, float angle, float tol=1e-2)
{
  world.resetTime();
  const float tau = 2;
  const float startTime = world.getTime();
  while (true) {
    float error = engine->GetJointAngle() - angle;
    if (fabs(error)<tol) break;
    engine->SetMotorSpeed(-error/tau);
    world.stepWorld();
  }
  const float endTime = world.getTime();
  qDebug() << "angle" << engine->GetJointAngle()*180/b2_pi << "time" << (endTime-startTime);
}

b2Body *buildRobot(World &world, const b2Vec2 &center, b2Body *ground)
{
  const float motorRadius = 1.5;
  const float mainLength  = 20.;
  const float mainHeight  = 2;
  const float upperExtension = 2;
  const float legWidth = 4;
  const float legHeight = 5;
  const float footHeight = 4;
  b2Body* main = world.addBox(center,mainLength,mainHeight);
  b2Joint* fix0 = world.addHingeJoint(main,ground,center-b2Vec2(mainLength/3.,0));
  b2Joint* fix1 = world.addHingeJoint(main,ground,center+b2Vec2(mainLength/3.,0));

  b2Body* motor = world.addBall(center,motorRadius);
  b2RevoluteJoint* engine = static_cast<b2RevoluteJoint*>(world.addHingeJoint(motor,main,motor->GetWorldCenter(),false,10000,0));

  { // left leg
    b2Vec2 baseVec = center + b2Vec2(-mainLength/2.,0);
    b2Body* upperPart = world.addBox(baseVec + b2Vec2(-legWidth/2.,(motorRadius+upperExtension)/2.),legWidth,motorRadius+upperExtension);
    world.addHingeJoint(upperPart,main,baseVec);
    world.addDistanceJoint(motor,upperPart,motor->GetWorldCenter()+b2Vec2(0,motorRadius),baseVec + b2Vec2(0,motorRadius+upperExtension),true);
    b2Body* lowerPart = world.addBox(baseVec + b2Vec2(-legWidth/2.,-legHeight-footHeight/2.),legWidth,footHeight);
    world.addDistanceJoint(upperPart,lowerPart,baseVec                      ,baseVec + b2Vec2(0,-legHeight)        ,true);
    world.addDistanceJoint(upperPart,lowerPart,baseVec + b2Vec2(-legWidth,0),baseVec + b2Vec2(-legWidth,-legHeight),true);
    world.addDistanceJoint(motor,lowerPart,motor->GetWorldCenter()+b2Vec2(0,motorRadius),baseVec + b2Vec2(0,-legHeight),true);
  }

  rotateEngine(world,engine,b2_pi);

  { // right leg
    b2Vec2 baseVec = center + b2Vec2(mainLength/2.,0);
    b2Body* upperPart = world.addBox(baseVec + b2Vec2(legWidth/2.,(motorRadius+upperExtension)/2.),legWidth,motorRadius+upperExtension);
    world.addHingeJoint(upperPart,main,baseVec);
    world.addDistanceJoint(motor,upperPart,motor->GetWorldCenter()+b2Vec2(0,-motorRadius),baseVec + b2Vec2(0,motorRadius+upperExtension),true);
    b2Body* lowerPart = world.addBox(baseVec + b2Vec2(legWidth/2.,-legHeight-footHeight/2.),legWidth,footHeight);
    world.addDistanceJoint(upperPart,lowerPart,baseVec                     ,baseVec + b2Vec2(0,-legHeight)        ,true);
    world.addDistanceJoint(upperPart,lowerPart,baseVec + b2Vec2(legWidth,0),baseVec + b2Vec2(legWidth,-legHeight),true);
    world.addDistanceJoint(motor,lowerPart,motor->GetWorldCenter()+b2Vec2(0,-motorRadius),baseVec + b2Vec2(0,-legHeight),true);
  }

  //world.destroyJoint(fix0);
  //world.destroyJoint(fix1);
  engine->SetMotorSpeed(b2_pi/2.);
  return main;
}

int main(int argc,char * argv[])
{
  QApplication app(argc,argv);

  World world;
  {
    world.initialize(b2Vec2(0,-10));
    b2Body* ground = world.addGround();
    //b2Body* poutre = world.addBox(-15,8,10,.2);
    //world.addDistanceJoint(poutre,ground,b2Vec2(-19.9,8),b2Vec2(-19.9,-0.5),true);
    //world.addDistanceJoint(poutre,ground,b2Vec2(-10.1,8),b2Vec2(-10.1,-0.5),true);
    //world.addBox(-10,10);
    //world.addBox(3,10);
    //world.addBox(-1,7);
    //world.addBall(0,5);
    buildRobot(world,b2Vec2(0,12),ground);
  }


  //Printer printer;
  Drawer drawer;
  drawer.resize(1000,500);
  drawer.show();

  world.setStepping(true);

  QObject::connect(&world,SIGNAL(worldStepped(World*)),&drawer,SLOT(displayWorld(World*)));
  //QObject::connect(&world,SIGNAL(worldStepped(World*)),&printer,SLOT(displayWorld(World*)));
  //QObject::connect(&printer,SIGNAL(done()),&app,SLOT(quit()));

  return app.exec();
}

