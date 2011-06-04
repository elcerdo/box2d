#ifndef __PRINTER_H__
#define __PRINTER_H__

#include <QObject>
#include <QTimer>
#include "world.h"

#include <QDebug>

class Printer : public QObject {
Q_OBJECT
public:
  Printer(QObject *parent=NULL);

  void start(int step);

public slots:
  void bang();
  void displayWorld();

signals:
  void done();

protected:
  World *world;
  QTimer *timer;
  int stepLeft;
};

#endif
