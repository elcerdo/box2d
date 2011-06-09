#ifndef __ROBOT_H__
#define __ROBOT_H__

#include <QObject>
#include "pickling/chooseser.h"
#include "Box2D/Box2D.h"

class World; // forward

struct RobotDef {
  RobotDef();

  void loadFromFile(const std::string &filename);
  Tab  getDict() const;
  void print() const;

  float motorRadius;
  float mainLength;
  float mainHeight;
  float upperExtension;
  float legWidth;
  float legHeight;
  float legAngle;
  float footHeight;
  int   legNumber;
};

struct Robot {
  Robot(const RobotDef &robotDef, b2Body* main, b2RevoluteJoint *engine);

  const RobotDef robotDef;
  b2Body* main;
  b2RevoluteJoint* engine;
};

class RobotTimer : public QObject {
Q_OBJECT
public:
  RobotTimer(const Robot &robot, QObject *parent=NULL);
  ~RobotTimer();

  void setRange(float xmin, float xmax);
public slots:
  void analyseWorld(World* world);
  void printReport() const;
  void saveReport(const std::string &filename) const;
signals:
  void done();
protected:
  struct Record {
    b2Vec2 position;
    b2Vec2 speed;
    float bodyangle;
    float engineangle;
    float time;
  };
  typedef QList<Record> Records;
  Records records;

  const Robot &robot;
  float xmin,xmax;
  bool started,stopped;
};


#endif 

