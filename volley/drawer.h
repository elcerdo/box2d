#ifndef __DRAWER_H__
#define __DRAWER_H__

#include <QDebug>
#include <QWidget>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include "world.h"

class Drawer : public QWidget {
Q_OBJECT
public:
  Drawer(QWidget *parent=NULL);
  ~Drawer();

public slots:
  void displayWorld(World* world);

protected:
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

};

#endif
