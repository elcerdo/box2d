#include "common.h"

BadRobot::BadRobot(const RobotDef &robotDef, Type type)
: std::exception(), type(type), robotDef(robotDef)
{
}

const BadRobot::Type BadRobot::getType() const throw()
{
  return type;
}

const RobotDef &BadRobot::getRobotDef() const throw()
{
  return robotDef;
}

const char* BadRobot::what() const throw()
{
  switch (type) {
  case BAD_ROTATION:
    return "rotation took too long";
  case TOO_LONG:
    return "transition took too long";
  case BAD_BEHAVIOR:
    return "bad robot behavior";
  case NO_ERROR:
    return "no error";
  }
}

QPointF toQPointF(const b2Vec2 &vect)
{
  return QPointF(vect.x,vect.y);
}

