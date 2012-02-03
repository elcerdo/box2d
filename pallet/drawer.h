#ifndef __DRAWER_H__
#define __DRAWER_H__

#include <QTime>
#include <QPixmap>
#include <QDebug>
#include <QGLWidget>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include "world.h"
#include "gamestate.h"

//class Drawer : public QGLWidget {
class Drawer : public QWidget {
	Q_OBJECT
	public:
		Drawer(GameState &state,QWidget *parent=NULL);
		~Drawer();

	public slots:
		void displayWorld(World* world);

	protected:
		void keyPressEvent(QKeyEvent* event);
		void keyReleaseEvent(QKeyEvent* event);
		void paintEvent(QPaintEvent* event);
		void mousePressEvent(QMouseEvent* event);
		void mouseMoveEvent(QMouseEvent* event);
		void mouseReleaseEvent(QMouseEvent* event);
		void wheelEvent(QWheelEvent* event);

	protected:
		World* world;
		GameState& state;

		bool panning;
		bool fullscreen;
		QPointF panningPosition;
		QPointF panningPositionStart;
		QPointF panningPositionCurrent;
		float scale;
		bool debugdraw;
		float cursorMovedTime;

		QPen debugPen;
		QFont debugFont;

		typedef QList<int> TimeStamps;
		TimeStamps frame_stamps;
		QTime time;

	signals:
		void exitButtonPressed();
};

#endif
