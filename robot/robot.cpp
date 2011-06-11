#include "robot.h"

#include "world.h"
#include <iostream>
using std::cout;
using std::endl;

RobotDef::RobotDef()
{
  motorRadius = 1.5;
  mainLength  = 10.;
  mainHeight  = 2;
  upperExtension = 3;
  legWidth = 3;
  legHeight = 5;
  legAngle = 15/180.*b2_pi;
  footHeight = 5;
  legNumber = 4;
}

void RobotDef::loadFromFile(const std::string &filename)
{
  Val data;
  LoadValFromFile(filename,data);
  Tab dict = data;

  motorRadius = dict["motorRadius"];
  mainLength  = dict["mainLength"];
  mainHeight  = dict["mainHeight"];
  upperExtension = dict["upperExtension"];
  legWidth = dict["legWidth"];
  legHeight = dict["legHeight"];
  legAngle = dict["legAngle"];
  footHeight = dict["footHeight"];
  legNumber = dict["legNumber"];
}

Tab RobotDef::getDict() const {
  Tab dict;
  dict["motorRadius"] = motorRadius;
  dict["mainLength"] = mainLength;
  dict["mainHeight"] = mainHeight;
  dict["upperExtension"] = upperExtension;
  dict["legWidth"] = legWidth;
  dict["legHeight"] = legHeight;
  dict["legAngle"] = legAngle;
  dict["footHeight"] = footHeight;
  dict["legNumber"] = legNumber;
  return dict;
}

void RobotDef::print() const
{
  cout << "**** DEFINITION ****" << endl;
  cout << "motorRadius = " << motorRadius << endl;
  cout << "mainLength  = " << mainLength << endl;
  cout << "mainHeight  = " <<  mainHeight << endl;
  cout << "upperExtension = " <<  upperExtension << endl;
  cout << "legWidth = " <<  legWidth << endl;
  cout << "legHeight = " <<  legHeight << endl;
  cout << "legAngle = " <<  legAngle << endl;
  cout << "footHeight = " <<  footHeight << endl;
  cout << "legNumber = " <<  legNumber << endl;
}


Robot::Robot(const RobotDef &robotDef, b2Body* main, b2RevoluteJoint *engine)
: robotDef(robotDef), main(main), engine(engine)
{
}

RobotTimer::RobotTimer(const Robot* robot, QObject *parent)
: QObject(parent), robot(NULL), xmin(0), xmax(0), started(false), stopped(false)
{
}

RobotTimer::~RobotTimer()
{
}

void RobotTimer::setRobot(const Robot* robot)
{
  this->robot = robot;
}

void RobotTimer::analyseWorld(World* world)
{
  Q_ASSERT(robot);

  Record record;
  record.position = robot->main->GetPosition();
  record.speed = robot->main->GetLinearVelocity();
  record.bodyangle = robot->main->GetAngle()*180/b2_pi;
  record.engineangle = robot->engine->GetJointAngle()*180/b2_pi;
  record.time = world->getTime();

  if (fabs(record.bodyangle)>25) throw BadRobot(robot->robotDef,BadRobot::BAD_BEHAVIOR);

  if (record.position.x<xmin || record.position.x>xmax) {
    if (started && !stopped) {
      cout << "**** RECORDING FINISHED ****" << endl;
      stopped = true;
      printReport();
      emit done();
    }
    return;
  }

  if (!started) {
    cout << "**** STARTED RECORDING ****" << endl;
    started = true;
  }
  records.push_back(record);
}

void RobotTimer::setRange(float xmin, float xmax)
{
  this->xmin = xmin;
  this->xmax = xmax;
}

void RobotTimer::saveReport(const std::string &filename,const BadRobot &badRobot) const
{
  cout << "**** SAVING PERFORMANCES ****" << endl;
  cout << "status = " << badRobot.getType() << " (" << badRobot.what() << ")" << endl;
  cout << "filename = " << filename << endl;

  Tab dict;
  dict["definition"] = badRobot.getRobotDef().getDict();
  dict["status"] = badRobot.getType();
  dict["status_string"] = badRobot.what();

  Arr posx,posy;
  Arr spex,spey;
  Arr bodyangle;
  Arr engineangle;
  Arr time;
  for (Records::const_iterator irecord=records.begin(); irecord!=records.end(); irecord++) {
    posx.append(irecord->position.x);
    posy.append(irecord->position.y);
    spex.append(irecord->speed.x);
    spey.append(irecord->speed.y);
    bodyangle.append(irecord->bodyangle);
    engineangle.append(irecord->engineangle);
    time.append(irecord->time);
  }
  dict["posx"] = posx;
  dict["posy"] = posy;
  dict["spex"] = spex;
  dict["spey"] = spey;
  dict["bodyangle"] = bodyangle;
  dict["engineangle"] = engineangle;
  dict["time"] = time;

  DumpValToFile(dict,filename);
}

void RobotTimer::printReport() const
{
  cout << "**** PRINTING PERFORMANCES ****" << endl;
  cout << "nrecord = " << records.size() << endl;

  b2Vec2 meanSpeed(0,0);
  b2Vec2 minSpeed(0,0);
  b2Vec2 maxSpeed(0,0);
  b2Vec2 squaredSpeed(0,0);
  bool first = true;
  for (Records::const_iterator irecord=records.begin(); irecord!=records.end(); irecord++) {
    meanSpeed += irecord->speed;
    squaredSpeed += b2Vec2(irecord->speed.x*irecord->speed.x,irecord->speed.y*irecord->speed.y);
    if (first) {
      minSpeed = irecord->speed;
      maxSpeed = irecord->speed;
      first = false;
    } else {
      if (irecord->speed.Length()<minSpeed.Length()) minSpeed = irecord->speed;
      if (irecord->speed.Length()>maxSpeed.Length()) maxSpeed = irecord->speed;
    }
  }
  meanSpeed /= records.size();
  squaredSpeed /= records.size();
  squaredSpeed = b2Vec2(sqrt(squaredSpeed.x-meanSpeed.x*meanSpeed.x),sqrt(squaredSpeed.y-meanSpeed.y*meanSpeed.y));

  cout << "meanspeed = " << meanSpeed << endl;
  cout << "stdspeed = " << squaredSpeed << endl;
  cout << "minspeed = " << minSpeed << endl;
  cout << "maxspeed = " << maxSpeed << endl;
}

