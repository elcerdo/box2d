#ifndef __COMMON_H__
#define __COMMON_H__

#include <exception>
#include <QPointF>
#include "Box2D/Box2D.h"

class RobotDef; // forward

class BadRobot : public std::exception {
public:
  enum Type {NO_ERROR,BAD_ROTATION,TOO_LONG,BAD_BEHAVIOR};
  BadRobot(const RobotDef& robotDef, Type type);
  const char* what() const throw();
  const Type getType() const throw();
  const RobotDef &getRobotDef() const throw();
protected:
  Type type;
  const RobotDef& robotDef;
};

template <class stream>
stream& operator<<(stream &os, const b2Vec2 &vect)
{
  return os << "(" << vect.x << "," << vect.y << ")";
}

QPointF toQPointF(const b2Vec2 &vect);

#endif

