#ifndef __COMMON_H__
#define __COMMON_H__

#include <Box2D/Box2D.h>
#include <QString>
#include <QPointF>

static const int nplayers = 2;

template <class Stream>
Stream& operator<<(Stream &os, const b2Vec2 &vect)
{
  return os << "(" << vect.x << "," << vect.y << ")";
}

QPointF toQPointF(const b2Vec2 &vect);
QString keyToString(int key);

#endif

