#include "common.h"

BadRobot::BadRobot(Type type)
: std::exception(), type(type)
{
}

const char* BadRobot::what() const throw()
{
  switch (type) {
  case BAD_ROTATION:
    return "rotation took too long";
  case TOO_LONG:
    return "transition took too long";
  }
}
QPointF toQPointF(const b2Vec2 &vect)
{
  return QPointF(vect.x,vect.y);
}

