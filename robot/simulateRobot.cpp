#include <QApplication>
#include "drawer.h"
#include "world.h"
#include "logic.h"
#include "robot.h"

int main(int argc,char * argv[])
{
  if (argc<3) {
    cerr << "simulateRobot definition.pck performance.pck" << endl;
    exit(2);
  }
  std::string input_filename = argv[1];
  std::string output_filename = argv[2];

  RobotDef robotDef;
  robotDef.loadFromFile(input_filename);
  robotDef.print();

  QApplication app(argc,argv);

  World world;
  world.initialize(b2Vec2(0,-10));

  RobotTimer robotTimer;
  robotTimer.setRange(0,50);

  Logic logic;

  try {
    b2Body* ground = world.addGround();
    Robot robot = world.addRobot(b2Vec2(-30,0),robotDef,ground);
    robotTimer.setRobot(&robot);
    logic.setRobot(&robot);

    Drawer drawer;
    drawer.resize(1000,500);
    drawer.show();


    QObject::connect(&world,SIGNAL(worldStepped(World*)),&drawer,SLOT(displayWorld(World*)));
    QObject::connect(&world,SIGNAL(worldStepped(World*)),&robotTimer,SLOT(analyseWorld(World*)));
    QObject::connect(&world,SIGNAL(worldStepped(World*)),&logic,SLOT(analyseWorld(World*)));
    QObject::connect(&robotTimer,SIGNAL(done()),&app,SLOT(quit()));

    world.setStepping(true);

    int code = app.exec();
    Q_ASSERT(code==0);
    robotTimer.saveReport(output_filename,BadRobot(robotDef,BadRobot::NO_ERROR));
    return code;
  } catch (BadRobot badRobot) {
    robotTimer.saveReport(output_filename,badRobot);
    return 1;
  }
}

