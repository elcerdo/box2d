#include "drawer.h"
#include "common.h"
#include <QPainter>
#include <QColor>

Drawer::Drawer(QWidget *parent)
: QWidget(parent), world(NULL)
{
}

void Drawer::displayWorld(World* world)
{
  this->world = world;
  update();
}

void Drawer::paintEvent(QPaintEvent* event)
{
  if (!world) return;

  QPainter painter(this);
  painter.translate(rect().width()/2.,rect().height()/2.);
  painter.scale(10,-10);

  for (const b2Body* body=world->getFirstBody(); body!=NULL; body=body->GetNext()) {
    painter.save();
    painter.translate(toQPointF(body->GetPosition()));
    painter.rotate(body->GetAngle()*180/M_PI);

    if (body->IsAwake()) painter.setBrush(QBrush(QColor::fromRgbF(1,0,0,.3)));
    else painter.setBrush(QBrush(QColor::fromRgbF(0,0,1,.3)));

    for (const b2Fixture* fixture=body->GetFixtureList(); fixture!=NULL; fixture=fixture->GetNext()) {
      if (fixture->GetShape()->GetType()==b2Shape::e_polygon) {
	const b2PolygonShape* shape = static_cast<const b2PolygonShape*>(fixture->GetShape());
	int vertexCount = shape->GetVertexCount();
	QPolygonF polygon;
	for (int kk=0; kk<vertexCount; kk++) { polygon << toQPointF(shape->GetVertex(kk)); }
	painter.drawPolygon(polygon);
      } else if (fixture->GetShape()->GetType()==b2Shape::e_circle) {
	const b2CircleShape* shape = static_cast<const b2CircleShape*>(fixture->GetShape());
	painter.drawEllipse(QRectF(-shape->m_radius,-shape->m_radius,2.*shape->m_radius,2.*shape->m_radius));
	painter.drawLine(QPointF(0,0),QPointF(shape->m_radius,0));
      } else Q_ASSERT(false);
    }

    painter.restore();
  }

}

