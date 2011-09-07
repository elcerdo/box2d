#include "drawer.h"
#include "common.h"
#include <QPainter>
#include <QColor>
#include <QSettings>

static const Qt::MouseButton panningButton = Qt::MidButton;

// standard keys
static const int leftPlayerStartKey = Qt::Key_Space;
static const int leftPlayerLeftKey = Qt::Key_A;
static const int leftPlayerRightKey = Qt::Key_Z;
static const int rightPlayerStartKey = Qt::Key_M;
static const int rightPlayerLeftKey = Qt::Key_Left;
static const int rightPlayerRightKey = Qt::Key_Right;
static const int fullscreenKey = Qt::Key_F;

//// alternate keys
//static const int leftPlayerStartKey = Qt::Key_S;
//static const int leftPlayerLeftKey = Qt::Key_D;
//static const int leftPlayerRightKey = Qt::Key_F;
//static const int rightPlayerStartKey = Qt::Key_J;
//static const int rightPlayerLeftKey = Qt::Key_G;
//static const int rightPlayerRightKey = Qt::Key_H;
//static const int fullscreenKey = Qt::Key_Space;

//scale is in pixel/m

Drawer::Drawer(GameData& data,QWidget *parent)
: QWidget(parent), world(NULL), panning(false), panningPosition(0,0), panningPositionStart(0,0), panningPositionCurrent(0,0), scale(0.), data(data)
{
    QSettings settings;
    scale = settings.value("drawer/scale",50.).toFloat();
    panningPosition = settings.value("drawer/panningPosition",0.).toPointF();

    qDebug() << "******************";
    qDebug() << "left player";
    qDebug() << "start" << QKeySequence(leftPlayerStartKey).toString();
    qDebug() << "left" << QKeySequence(leftPlayerLeftKey).toString();
    qDebug() << "right" << QKeySequence(leftPlayerRightKey).toString();
    qDebug() << "******************";
    qDebug() << "right player";
    qDebug() << "start" << QKeySequence(rightPlayerStartKey).toString();
    qDebug() << "right" << QKeySequence(rightPlayerLeftKey).toString();
    qDebug() << "right" << QKeySequence(rightPlayerRightKey).toString();
    qDebug() << "******************";
    qDebug() << "fullscreen" << QKeySequence(fullscreenKey).toString();
    qDebug() << "******************";
}

Drawer::~Drawer()
{
    QSettings settings;
    settings.setValue("drawer/scale",scale);
    settings.setValue("drawer/panningPosition",panningPosition);
}

void Drawer::displayWorld(World* world)
{
  this->world = world;
  update();
}

void Drawer::keyPressEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat()) {
	event->ignore();
	return;
    }

    //qDebug() << "pressed" << event->key();

    if (event->key()==fullscreenKey) {
	setWindowState(windowState() ^ Qt::WindowFullScreen);
	event->accept();
	return;
    }

    if (event->key()==leftPlayerStartKey) {
	data.leftPlayerStart();
	event->accept();
	return;
    }

    if (event->key()==rightPlayerStartKey) {
	data.rightPlayerStart();
	event->accept();
	return;
    }

    if (event->key()==leftPlayerLeftKey) {
	data.leftPlayerGoLeft();
	event->accept();
	return;
    }

    if (event->key()==leftPlayerRightKey) {
	data.leftPlayerGoRight();
	event->accept();
	return;
    }

    if (event->key()==rightPlayerLeftKey) {
	data.rightPlayerGoLeft();
	event->accept();
	return;
    }

    if (event->key()==rightPlayerRightKey) {
	data.rightPlayerGoRight();
	event->accept();
	return;
    }

    event->ignore();
}

void Drawer::keyReleaseEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat()) {
	event->ignore();
	return;
    }

    //qDebug() << "released" << event->key();

    if (event->key()==leftPlayerLeftKey) {
	data.leftPlayerStopLeft();
	event->accept();
	return;
    }

    if (event->key()==leftPlayerRightKey) {
	data.leftPlayerStopRight();
	event->accept();
	return;
    }

    if (event->key()==rightPlayerLeftKey) {
	data.rightPlayerStopLeft();
	event->accept();
	return;
    }

    if (event->key()==rightPlayerRightKey) {
	data.rightPlayerStopRight();
	event->accept();
	return;
    }


    event->ignore();
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

