
#include <QApplication>
#include "world.h"
#include "drawer.h"

int main(int argc, char *argv[]) {
    QApplication app(argc,argv);

    { // set details for QSettings
	QCoreApplication::setOrganizationName("softpower");
	QCoreApplication::setOrganizationDomain("softpower.ord");
	QCoreApplication::setApplicationName("volley");
    }

    World world(1./60.);
    world.initialize(b2Vec2(0,-10));

    { // build court
	const float court_width = 20;
	const float court_height = 20;
	const float court_net_height = 1;

	b2Body* left_ground = world.addGround(-court_width/4.,-.5,court_width/2.,1);
	b2Body* right_ground = world.addGround(court_width/4.,-.5,court_width/2.,1);
	b2Body* ceiling = world.addGround(0,court_height+.5,court_width,1);
	b2Body* left_wall = world.addGround(court_width/2.+.5,court_height/2.,1,court_height+2.);
	b2Body* right_wall = world.addGround(-court_width/2.-.5,court_height/2.,1,court_height+2.);
	b2Body* net = world.addGround(0,court_net_height/2.,0.05,court_net_height);
	b2Body* ball = world.addBall(0,4,.1);
    }

    Drawer drawer;
    QObject::connect(&world,SIGNAL(worldStepped(World*)),&drawer,SLOT(displayWorld(World*)));
    drawer.show();

    //world.stepWorld();
    world.setStepping(true);

    return app.exec();
}
