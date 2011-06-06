#include "common.h"

QDebug& operator<<(QDebug &os, const b2Vec2 &vect)
{
  return os << "(" << vect.x << "," << vect.y << ")";
}

QPointF toQPointF(const b2Vec2 &vect)
{
  return QPointF(vect.x,vect.y);
}
