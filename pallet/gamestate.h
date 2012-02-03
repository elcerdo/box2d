#ifndef __GAMESTATE_H__
#define __GAMESTATE_H__

#include <QObject>
#include "world.h"

class GameState : public QObject
{
	Q_OBJECT
	public:
		GameState(World& world);
	
	public slots:
		void stabilizeBalls(World* world);

	protected:
		World& world;
		b2Body* palet;
};

#endif
