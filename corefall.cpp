#include <QCoreApplication>
#include "printer.h"

int main(int argc,char * argv[])
{
  QCoreApplication app(argc,argv);

  Printer printer;
  printer.start(600);
  QObject::connect(&printer,SIGNAL(done()),&app,SLOT(quit()));

  return app.exec();
}

