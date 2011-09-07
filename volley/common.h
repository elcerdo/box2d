#ifndef __COMMON_H__
#define __COMMON_H__

#include <Box2D/Box2D.h>
#include <QPointF>

template <class stream>
stream& operator<<(stream &os, const b2Vec2 &vect)
{
  return os << "(" << vect.x << "," << vect.y << ")";
}

QPointF toQPointF(const b2Vec2 &vect);

#endif

