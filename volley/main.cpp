
#include <QApplication>
#include "world.h"
#include "gamedata.h"
#include "drawer.h"

int main(int argc, char *argv[]) {
    QApplication app(argc,argv);

    { // set details for QSettings
	QCoreApplication::setOrganizationName("softpower");
	QCoreApplication::setOrganizationDomain("softpower.ord");
	QCoreApplication::setApplicationName("volley");
    }

    World world(1./30.);
    world.initialize(b2Vec2(0,-10));

    GameData data(world);
    QObject::connect(&world,SIGNAL(postStepWorld(World*)),&data,SLOT(stabilizePlayers(World*)));

    Drawer drawer(data);
    QObject::connect(&world,SIGNAL(worldStepped(World*)),&drawer,SLOT(displayWorld(World*)));
    drawer.resize(800,600);
    drawer.show();

    //world.stepWorld();
    world.setStepping(true);

    return app.exec();
}
