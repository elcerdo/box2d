#include "drawer.h"
#include "common.h"
#include <QPainter>
#include <QColor>
#include <QSettings>

static const Qt::MouseButton panningButton = Qt::MidButton;

// standard keys
static const int leftPlayerStartKey = Qt::Key_Space;
static const int leftPlayerLeftKey = Qt::Key_Q;
static const int leftPlayerRightKey = Qt::Key_D;
static const int leftPlayerUpKey = Qt::Key_Z;

static const int rightPlayerStartKey = Qt::Key_M;
static const int rightPlayerLeftKey = Qt::Key_Left;
static const int rightPlayerRightKey = Qt::Key_Right;
static const int rightPlayerUpKey = Qt::Key_Up;

static const int fullscreenKey = Qt::Key_F;
static const int resetViewKey = Qt::Key_R;
static const int debugDrawKey = Qt::Key_T;

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
: QWidget(parent), world(NULL),
  panning(false), panningPosition(0,0), panningPositionStart(0,0), panningPositionCurrent(0,0), scale(0.), debug_draw(false),
  data(data),
  ballImage(":/images/ball.png"), leftPlayerImage(":/images/left_blob_00.png"), rightPlayerImage(":/images/right_blob_00.png"),
  poleImage(":/images/pole.png"), backgroundImage(":/images/beach.jpg")
{
    setFixedSize(800,600);
    QSettings settings;
    scale = settings.value("drawer/scale",40.).toFloat();
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
    qDebug() << "resetview" << QKeySequence(resetViewKey).toString();
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

    if (event->key()==resetViewKey) {
	panningPosition = QPointF(0,0);
	scale = 40;
	update();
	event->accept();
	return;
    }

    if (event->key()==debugDrawKey) {
	debug_draw = !debug_draw;
	update();
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

    if (event->key()==leftPlayerUpKey) {
      data.leftPlayerJump();
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

    if (event->key()==rightPlayerUpKey) {
      data.rightPlayerJump();
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

    if (event->key()==leftPlayerUpKey) {
	data.leftPlayerStopJump();
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

    if (event->key()==rightPlayerUpKey) {
	data.rightPlayerStopJump();
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
  painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform,true);

  { // draw scene
      painter.save();
      painter.translate(rect().width()/2.,rect().height());
      painter.translate(panningPosition);
      if (panning) painter.translate(panningPositionCurrent-panningPositionStart);
      painter.scale(scale,-scale);

      { // draw background
          painter.save();
          painter.scale(1,-1);
          painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform,false);
          painter.drawImage(QRectF(-data.courtWidth()/2,-15,data.courtWidth(),15),backgroundImage);
          painter.restore();
      }

      { // draw ball
	  const b2Body *ball = data.getBall();
	  painter.save();
	  painter.translate(toQPointF(ball->GetPosition()));
	  painter.rotate(ball->GetAngle()*180/b2_pi);
	  painter.drawImage(QRectF(-data.ballRadius(),-data.ballRadius(),2*data.ballRadius(),2*data.ballRadius()),ballImage);
	  painter.restore();
      }

      { // draw left player
	  const b2Body *player = data.getLeftPlayer();
	  painter.save();
	  painter.translate(toQPointF(player->GetPosition()));
	  painter.scale(1,-1);
	  painter.drawImage(QRectF(-data.playerRadius(),-data.playerRadius(),2*data.playerRadius(),data.playerRadius()),leftPlayerImage);
	  painter.restore();
      }

      { // draw right player
	  const b2Body *player = data.getRightPlayer();
	  painter.save();
	  painter.translate(toQPointF(player->GetPosition()));
	  painter.scale(1,-1);
	  painter.drawImage(QRectF(-data.playerRadius(),-data.playerRadius(),2*data.playerRadius(),data.playerRadius()),rightPlayerImage);
	  painter.restore();
      }

      { // draw pole
	  painter.save();
	  painter.scale(1,-1);
	  QPen pen;
	  pen.setColor(qRgb(100,102,105));
	  pen.setWidthF(.05);
	  painter.setPen(pen);
	  painter.drawLine(QPointF(1,0),QPointF(0,-2.5));
	  painter.drawLine(QPointF(-1,0),QPointF(0,-2.5));
	  painter.drawImage(QRectF(-data.netWidth()/2.,-data.netHeight(),data.netWidth(),data.netHeight()),poleImage);
	  painter.restore();
      }

      if (debug_draw) { // debug draw scene
	  // draw bodies
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

	  { // draw joints
	      painter.save();
	      painter.setPen(QPen(QColor::fromRgbF(0,1,0)));
	      for (const b2Joint* joint=world->getFirstJoint(); joint!=NULL; joint=joint->GetNext()) {
		  painter.drawLine(toQPointF(joint->GetAnchorA()),toQPointF(joint->GetAnchorB()));
	      }
	      painter.restore();
	  }
      }

      painter.restore();
  }

  { // draw overlay
      painter.save();
      QFont font;
      font.setBold(true);
      font.setPixelSize(100);
      painter.setFont(font);
      QRectF score(width()/2.-150,10,300,100);
      painter.setBrush(QColor("yellow"));
      painter.drawRect(score);
      painter.setBrush(QColor("black"));
      painter.drawText(score,Qt::AlignCenter,QString("%1 - %2").arg(data.leftPlayerScore()).arg(data.rightPlayerScore()));
      painter.restore();
  }


}

