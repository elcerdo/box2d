#ifndef __PRINTER_H__
#define __PRINTER_H__

#include <QObject>
#include "world.h"

class Printer : public QObject {
Q_OBJECT
public:
  Printer(QObject *parent=NULL);

public slots:
  void displayWorld(World* world);

signals:
  void done();

};

#endif
