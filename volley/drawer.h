#ifndef __DRAWER_H__
#define __DRAWER_H__

#include <QTime>
#include <QPixmap>
#include <QDebug>
#include <QGLWidget>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include "world.h"
#include "gamedata.h"
#include "keymanager.h"

class Drawer : public QGLWidget {
Q_OBJECT
public:
  Drawer(GameData &data,QWidget *parent=NULL);
  ~Drawer();

public slots:
  void displayWorld(World* world);
  void recordPhysicsStamp(World* world);

protected:
  void keyPressEvent(QKeyEvent* event);
  void keyReleaseEvent(QKeyEvent* event);
  void paintEvent(QPaintEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void wheelEvent(QWheelEvent* event);
  void resizeEvent(QResizeEvent* event);

  static bool handlePlayerKeyPress(int key, const PlayerKeys& keys, Player& player, float time);
  static bool handlePlayerKeyRelease(int key, const PlayerKeys& keys, Player& player, float time);

protected:
  void drawPlayerName(const Player& player, QPainter &painter) const;
  void resetView();

  World* world;

  bool panning;
  bool fullscreen;
  QPointF panningPosition;
  QPointF panningPositionStart;
  QPointF panningPositionCurrent;
  float scale;
  bool debugdraw;
  float cursorMovedTime;

  typedef QList<int> TimeStamps;
  QTime time;
  TimeStamps frame_stamps;
  TimeStamps physics_stamps;

  GameData &data;

  QPixmap ballImage;
  QPixmap leftPlayerImage;
  QPixmap rightPlayerImage;
  QPixmap arrowImage;
  QPixmap trajectoryImage;
  QPixmap birdImage;
  //QPixmap win00,win01;
  //QPixmap lose00,lose01;

  QPixmap frame;
  QPixmap noises[40];
  int noise_current;

  QPen drawingPen;
  QFont drawingFont;
  QPen debugPen;
  QFont debugFont;

 signals:
  void exitButtonPressed();
};

#endif
