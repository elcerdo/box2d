#ifndef __COMMON_H__
#define __COMMON_H__

#include <QDebug>
#include <Box2D/Box2D.h>

const Qt::MouseButton panningButton = Qt::MidButton;
const float cursorHideTime = 1.5;

const Qt::Key fullscreenKey = Qt::Key_F;
const Qt::Key resetKey = Qt::Key_R;
const Qt::Key debugKey = Qt::Key_D;

const float boardSize = 1.4;
const float boardBorderWidth = .3;
const float ballRadius = .05;

inline QDebug operator<<(QDebug stream, const b2Vec2& vect)
{
	stream.nospace() << "[" << vect.x << "," << vect.y << "]";
	return stream.space();
}

#endif
