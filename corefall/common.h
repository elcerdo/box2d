#ifndef __COMMON_H__
#define __COMMON_H__

#include <QDebug>
#include <QPointF>
#include "Box2D/Box2D.h"
#include "world.h"

template <class stream>
stream& operator<<(stream &os, const b2Vec2 &vect)
{
  return os << "(" << vect.x << "," << vect.y << ")";
}

QPointF toQPointF(const b2Vec2 &vect);

#endif

