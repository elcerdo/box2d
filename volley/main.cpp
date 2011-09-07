
#include <QApplication>
#include "world.h"
#include "drawer.h"

int main(int argc, char *argv[]) {
    QApplication app(argc,argv);

    World world(1./60.);
    world.initialize(b2Vec2(0,-10));
    world.addGround();
    world.addBall(0,10,1);

    Drawer drawer;
    QObject::connect(&world,SIGNAL(worldStepped(World*)),&drawer,SLOT(displayWorld(World*)));
    drawer.show();

    //world.stepWorld();
    world.setStepping(true);

    return app.exec();
}
