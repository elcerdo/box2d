#ifndef __DRAWER_H__
#define __DRAWER_H__

#include <QDebug>
#include <QWidget>
#include <QPaintEvent>
#include "world.h"

class Drawer : public QWidget {
Q_OBJECT
public:
  Drawer(QWidget *parent=NULL);

public slots:
  void displayWorld(World* world);

protected:
  void paintEvent(QPaintEvent* event);

protected:
  World* world;
};

#endif
