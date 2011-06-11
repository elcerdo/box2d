#include "drawer.h"
#include "common.h"
#include <QPainter>
#include <QColor>

static const Qt::MouseButton panningButton = Qt::MidButton;

Drawer::Drawer(QWidget *parent)
: QWidget(parent), world(NULL), panning(false), panningPosition(0,0), panningPositionStart(0,0), panningPositionCurrent(0,0), scale(10.)
{
}

void Drawer::displayWorld(World* world)
{
  this->world = world;
  update();
}

void Drawer::mousePressEvent(QMouseEvent* event)
{
  if (event->button()==panningButton) {
    panning = true;
    panningPositionStart = event->posF();
    panningPositionCurrent = event->posF();
    event->accept();
  }
}

void Drawer::mouseMoveEvent(QMouseEvent* event)
{
  if (event->buttons().testFlag(panningButton) && panning) {
    panningPositionCurrent = event->posF();
    event->accept();
  }
}

void Drawer::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button()==panningButton) {
    panning = false;
    panningPositionCurrent = event->posF();
    panningPosition += panningPositionCurrent-panningPositionStart;
    event->accept();
  }
}

void Drawer::wheelEvent(QWheelEvent* event)
{
  double degree = event->delta()/8;
  scale *= pow(2,degree/180);
}

void Drawer::paintEvent(QPaintEvent* event)
{
  if (!world) return;

  QPainter painter(this);
  painter.translate(rect().width()/2.,rect().height()/2.);
  painter.translate(panningPosition);
  if (panning) painter.translate(panningPositionCurrent-panningPositionStart);
  painter.scale(scale,-scale);

  for (const b2Body* body=world->getFirstBody(); body!=NULL; body=body->GetNext()) {
    painter.save();
    painter.translate(toQPointF(body->GetPosition()));
    painter.rotate(body->GetAngle()*180/b2_pi);

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

  painter.setPen(QPen(QColor::fromRgbF(0,1,0)));
  for (const b2Joint* joint=world->getFirstJoint(); joint!=NULL; joint=joint->GetNext()) {
    painter.drawLine(toQPointF(joint->GetAnchorA()),toQPointF(joint->GetAnchorB()));
  }


}

