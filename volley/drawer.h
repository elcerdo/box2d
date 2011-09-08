#ifndef __DRAWER_H__
#define __DRAWER_H__

#include <QImage>
#include <QDebug>
#include <QWidget>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include "world.h"
#include "gamedata.h"

class Drawer : public QWidget {
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

protected:
  World* world;

  bool panning;
  QPointF panningPosition;
  QPointF panningPositionStart;
  QPointF panningPositionCurrent;
  float scale;
  bool debug_draw;

  GameData &data;

  QImage ballImage;
  QImage leftPlayerImage;
  QImage rightPlayerImage;
  QImage poleImage;
};

#endif
