
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

    { // dump key settings
	KeyManager keyManager;
	keyManager.dumpKeys(qDebug());
    }

    World world(1./50.);
    world.initialize(b2Vec2(0,-10));

    GameData data(world);
    QObject::connect(&world,SIGNAL(postStepWorld(World*)),&data,SLOT(stabilizePlayers(World*)));
    QObject::connect(&world,SIGNAL(preStepWorld(World*)),&data,SLOT(checkPoints(World*)));

    Drawer drawer(data);
    QObject::connect(&world,SIGNAL(worldStepped(World*)),&drawer,SLOT(displayWorld(World*)));
    drawer.show();

    //world.stepWorld();
    world.setStepping(true);

    return app.exec();
}
