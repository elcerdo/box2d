#ifndef __COMMON_H__
#define __COMMON_H__

#include <QDebug>
#include <QPointF>
#include "Box2D/Box2D.h"
#include "world.h"

QDebug& operator<<(QDebug &os, const b2Vec2 &vect);
QPointF toQPointF(const b2Vec2 &vect);

#endif

