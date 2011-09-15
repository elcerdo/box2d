#ifndef __DRAWER_H__
#define __DRAWER_H__

#include <QPixmap>
#include <QDebug>
#include <QGLWidget>
#include <QKeyEvent>
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

protected:
  void keyPressEvent(QKeyEvent* event);
  void keyReleaseEvent(QKeyEvent* event);
  void paintEvent(QPaintEvent* event);
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void wheelEvent(QWheelEvent* event);

  static bool handlePlayerKeyPress(int key, const PlayerKeys& keys, Player& player, float time);
  static bool handlePlayerKeyRelease(int key, const PlayerKeys& keys, Player& player, float time);

protected:
  World* world;

  bool panning;
  QPointF panningPosition;
  QPointF panningPositionStart;
  QPointF panningPositionCurrent;
  float scale;
  bool debugdraw;

  GameData &data;

  QPixmap ballImage;
  QPixmap leftPlayerImage;
  QPixmap rightPlayerImage;
  QPixmap poleImage;
  QPixmap backgroundImage;
  QPixmap arrowImage;
  QPixmap win00,win01;
  QPixmap lose00,lose01;

  QPixmap frame;
  QPixmap noises[10];
  int noise_current;
};

#endif
