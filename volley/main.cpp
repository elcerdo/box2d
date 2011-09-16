#include <QFontDatabase>
#include <QPixmapCache>
#include <QApplication>
#include "world.h"
#include "gamemanager.h"
#include "gamedata.h"
#include "drawer.h"

/*
 Calcul de
 
      sigma*\sqrt{12/n}(\sigma_{i=1}^{12} rand()/RAND_MAX - n/2) + mu

 pour n = 12
*/
double L_gauss(float mu, float sigma)
{
  double sum = 0.0;

  for (int i = 0; i < 12; i++)
    sum += (double)qrand();
  sum = sum/RAND_MAX - 6;

  return sigma*sum + mu;;
}

void generateBackgroundNoise()
{
    qDebug() << "generating background noise";
    QPixmapCache cache;

    for (int kk=0; kk<10; kk++) {
	QImage image(400,300,QImage::Format_RGB32);
	image.fill(qRgb(0,0,0));
	for (int x=0; x<image.width(); x++) {
	    for (int y=0; y<image.height(); y++) {
		if (qrand()%100>40) continue;
		// int value = qrand() % 45;
		// if (value>32) value = 32 + (value-32)*2;
		int value = (int) fabs(L_gauss(0., 30.));
		image.setPixel(x,y,qRgb(value,value,value));
	    }
	}
	QString name = QString("noise%1").arg(kk);
	QPixmap pixmap = QPixmap::fromImage(image);
	cache.insert(name,pixmap);
    }
}

void loadFont()
{
    qDebug() << "loading font";
    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/fonts/04b03.ttf");
    fontDB.addApplicationFont(":/fonts/PixelSplitter-Bold.ttf");
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    QApplication app(argc,argv);

    { // set details for QSettings
	QCoreApplication::setOrganizationName("softpower");
	QCoreApplication::setOrganizationDomain("softpower.ord");
	QCoreApplication::setApplicationName("volley");
    }

    // generate graphics
    generateBackgroundNoise();
    loadFont();

    // dump key settings
    KeyManager::dumpKeys();

    World world(1./50.);
    world.initialize(b2Vec2(0,-GameManager::gravity()));

    GameData data(world);
    QObject::connect(&world,SIGNAL(postStepWorld(World*)),&data,SLOT(stabilizePlayers(World*)));
    QObject::connect(&world,SIGNAL(postStepWorld(World*)),&data,SLOT(recordBallPosition(World*)));
    QObject::connect(&world,SIGNAL(preStepWorld(World*)),&data,SLOT(checkState(World*)));

    Drawer drawer(data);
    QObject::connect(&world,SIGNAL(worldStepped(World*)),&drawer,SLOT(displayWorld(World*)));
    drawer.show();

    QObject::connect(&drawer,SIGNAL(exitButtonPressed()), &app, SLOT(quit()));
    //world.stepWorld();
    world.setStepping(true);

    return app.exec();
}
