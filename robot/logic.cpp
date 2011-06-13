#include "logic.h"

#include "common.h"
#include <iostream>
using std::endl;
using std::cout;

Logic::Logic(Robot* robot, QObject *parent)
: QObject(parent), state(INIT), lastTransitionTime(0), robot(NULL)
{
}

void Logic::setRobot(Robot* robot)
{
  this->robot = robot;
}

void Logic::analyseWorld(World* world)
{
  Q_ASSERT(robot);

  switch (state) {
  case INIT:
    cout << "**** TO FALLING ****" << endl;
    state = FALLING;
    lastTransitionTime = world->getTime();
    break;
  case FALLING:
    if (world->getTime()-lastTransitionTime>15) throw BadRobot(robot->robotDef,BadRobot::TOO_LONG);
    if (world->allBodiesAsleep() || robot->main->GetLinearVelocity().Length()<1e-1) {
      state = RUNNING;
      cout << "**** TO RUNNING ****" << endl;
      cout << "time = " << (world->getTime()-lastTransitionTime) << endl;
      lastTransitionTime = world->getTime();
      robot->engine->SetMotorSpeed(b2_pi);
    }
    break;
  case RUNNING:
    if (world->getTime()-lastTransitionTime>60) throw BadRobot(robot->robotDef,BadRobot::TOO_LONG);
    break;
  }
}

