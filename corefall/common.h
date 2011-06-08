#ifndef __COMMON_H__
#define __COMMON_H__

#include <exception>
#include <QPointF>
#include "Box2D/Box2D.h"
#include "world.h"

class BadRobot : public std::exception {
public:
  enum Type {BAD_ROTATION,TOO_LONG};
  BadRobot(Type type);
  const char* what() const throw();
protected:
  Type type;
};

template <class stream>
stream& operator<<(stream &os, const b2Vec2 &vect)
{
  return os << "(" << vect.x << "," << vect.y << ")";
}

QPointF toQPointF(const b2Vec2 &vect);

#endif

