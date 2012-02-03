#include <QFontDatabase>
#include <QPixmapCache>
#include <QApplication>
#include "world.h"
#include "drawer.h"
#include "gamestate.h"

//void loadFont()
//{
//    qDebug() << "loading font";
//    QFontDatabase fontDB;
//    fontDB.addApplicationFont(":/fonts/04b03.ttf");
//    fontDB.addApplicationFont(":/fonts/PixelSplitter-Bold.ttf");
//}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    QApplication app(argc,argv);

	{ // set details for QSettings
		QCoreApplication::setOrganizationName("softpower");
		QCoreApplication::setOrganizationDomain("softpower.org");
		QCoreApplication::setApplicationName("pallet");
	}

    World world(1./60.);

	GameState state(world);
    QObject::connect(&world,SIGNAL(postStepWorld(World*)),&state,SLOT(stabilizeBalls(World*)));

    Drawer drawer(state);
    QObject::connect(&world,SIGNAL(worldStepped(World*)),&drawer,SLOT(displayWorld(World*)));
    drawer.show();

    QObject::connect(&drawer,SIGNAL(exitButtonPressed()), &app, SLOT(quit()));
    //world.stepWorld();
    world.setStepping(true);

    return app.exec();
}
