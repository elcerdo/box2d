#include "common.h"

QPointF toQPointF(const b2Vec2 &vect)
{
  return QPointF(vect.x,vect.y);
}

QString keyToString(int key)
{
    return QKeySequence(int).getString();
}
