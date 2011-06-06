#include "printer.h"
#include "common.h"

Printer::Printer(QObject *parent)
: QObject(parent)
{

}

void Printer::displayWorld(World *world) {
  World::Bodies bodies = world->getBodies();
  qDebug();
  qDebug() << "found" << bodies.size() << "body(ies)";
  bool allSleep = true;
  for (World::Bodies::const_iterator ibody=bodies.begin(); ibody!=bodies.end(); ibody++) {
    const b2Body *body = *ibody;
    allSleep &= !body->IsAwake();
    qDebug() << "awake" << body->IsAwake() << "position" << body->GetPosition() << "angle" << body->GetAngle()*180/M_PI;
    for (const b2Fixture* fixture=body->GetFixtureList(); fixture!=NULL; fixture=fixture->GetNext()) {
      qDebug() << "**" << "shapetype" << fixture->GetShape()->GetType();
      if (fixture->GetShape()->GetType()==b2Shape::e_polygon) {
	const b2PolygonShape* shape = static_cast<const b2PolygonShape*>(fixture->GetShape());
	int vertexCount = shape->GetVertexCount();
	qDebug() << "  " << "polygon" << "nvertex" << vertexCount << "radius" << shape->m_radius;
	for (int kk=0; kk<vertexCount; kk++) { qDebug() << "    " << shape->GetVertex(kk); }
      } else if (fixture->GetShape()->GetType()==b2Shape::e_circle) {
	const b2CircleShape* shape = static_cast<const b2CircleShape*>(fixture->GetShape());
	int vertexCount = shape->GetVertexCount();
	qDebug() << "  " << "circle" << "nvertex" << vertexCount << "radius" << shape->m_radius;
	for (int kk=0; kk<vertexCount; kk++) { qDebug() << "    " << shape->GetVertex(kk); }
      } else Q_ASSERT(false);
    }
  }

  if (allSleep) {
    emit done();
  }
}

