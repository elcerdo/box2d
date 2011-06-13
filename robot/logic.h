#ifndef __LOGIC_H__
#define __LOGIC_H__

#include <QObject>
#include "world.h"
#include "robot.h"

class Logic : public QObject {
Q_OBJECT
public:
  Logic(Robot* robot=NULL, QObject *parent=NULL);
  void setRobot(Robot* robot);
public slots:
  void analyseWorld(World* world);
protected:
  enum State {INIT,FALLING,RUNNING};
  State state;
  float lastTransitionTime;
  Robot* robot;
};

#endif
