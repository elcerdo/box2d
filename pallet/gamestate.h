#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

#include <QObject>
#include <QVector>
#include "world.h"

class GameState : public QObject
{
	Q_OBJECT
	public:
		GameState(World& world);
		void hitBall();

	public slots:
		void stabilizeBalls(World* world);

	protected:
		World& world;

		b2Body* addBall(float x, float y);
		typedef QVector<b2Body*> Bodies;
		Bodies balls;
		b2Body* hitted;
};

#endif
