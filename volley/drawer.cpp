#include "drawer.h"
#include "common.h"
#include <QPainter>
#include <QColor>
#include <QSettings>

static const Qt::MouseButton panningButton = Qt::MidButton;

// standard keys
static const int leftPlayerStartKey = Qt::Key_L;
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

static const int beginPointKey = Qt::Key_Space;
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
: QGLWidget(parent), world(NULL),
  panning(false), panningPosition(0,0), panningPositionStart(0,0), panningPositionCurrent(0,0), scale(0.), debugdraw(false),
  data(data),
  ballImage(":/images/ball.png"), leftPlayerImage(":/images/left_blob_00.png"), rightPlayerImage(":/images/right_blob_00.png"),
  poleImage(":/images/pole.png"), backgroundImage(":/images/beach.jpg"),
  arrowImage(":/images/arrow.png")
{
  resize(800,600);
  
    QSettings settings;
    scale = settings.value("drawer/scale",40.).toFloat();
    panningPosition = settings.value("drawer/panningPosition",0.).toPointF();
    debugdraw = settings.value("drawer/debugdraw",false).toBool();

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
    settings.setValue("drawer/debugdraw",debugdraw);
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
	scale = width()/data.courtWidth();
	update();
	event->accept();
	return;
    }

    if (event->key()==debugDrawKey) {
	debugdraw = !debugdraw;
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
      data.leftPlayerJump(world->getTime());
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
      data.rightPlayerJump(world->getTime());
      event->accept();
      return;
    }
    
    if(event->key() == beginPointKey){
      data.beginPoint();
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

  painter.translate(rect().width()/2.,rect().height());
  painter.translate(panningPosition);
  if (panning) painter.translate(panningPositionCurrent-panningPositionStart);
  painter.scale(scale,-scale);

  { // draw background
      painter.save();
      painter.scale(1,-1);
      painter.drawPixmap(QRectF(-data.courtWidth()/2,-data.sceneHeight(),data.courtWidth(),data.sceneHeight()),backgroundImage,backgroundImage.rect());
      painter.restore();
  }

  { // draw score overlay
      painter.save();
      painter.resetTransform();
      QFont font;
      font.setBold(true);
      font.setPixelSize(100);
      painter.setFont(font);
      QRectF score(width()/2.-180,40,300,100);
      painter.setBrush(QColor("yellow"));
      painter.drawRect(score);
      painter.setBrush(QColor("black"));
      painter.drawText(score,Qt::AlignCenter,QString("%1 - %2").arg(data.leftPlayerScore()).arg(data.rightPlayerScore()));
      painter.restore();
  }


  { // draw ball
      const b2Body *ball = data.getBall();

      painter.save();
      painter.translate(toQPointF(ball->GetPosition()));
      painter.scale(1,-1);
      painter.rotate(-ball->GetAngle()*180/b2_pi);
      painter.drawPixmap(QRectF(-data.ballRadius(),-data.ballRadius(),2*data.ballRadius(),2*data.ballRadius()),ballImage,ballImage.rect());
      painter.restore();


      if (ball->GetPosition().y>data.sceneHeight()+data.ballRadius()) {
	  float mini_ball_size = 1.-(ball->GetPosition().y-data.sceneHeight()-data.ballRadius())/15;
	  if (mini_ball_size<.2) mini_ball_size = .2;

	  painter.save();
	  painter.translate(QPointF(ball->GetPosition().x,data.sceneHeight()-.6));
	  painter.scale(1,-1);
	  painter.drawPixmap(QRectF(-.4,-.4,.8,.8),arrowImage,arrowImage.rect());
	  painter.translate(0,1);
	  painter.rotate(-ball->GetAngle()*180/b2_pi);
	  painter.drawPixmap(QRectF(-mini_ball_size/2.,-mini_ball_size/2.,mini_ball_size,mini_ball_size),ballImage,ballImage.rect());
	  painter.restore();
      }
  }

  { // draw left player
      const b2Body *player = data.getLeftPlayer();
      painter.save();
      painter.translate(toQPointF(player->GetPosition()));
      painter.scale(1,-1);
      painter.drawPixmap(QRectF(-data.playerRadius(),-data.playerRadius(),2*data.playerRadius(),data.playerRadius()),leftPlayerImage,leftPlayerImage.rect());
      painter.restore();
  }

  { // draw right player
      const b2Body *player = data.getRightPlayer();
      painter.save();
      painter.translate(toQPointF(player->GetPosition()));
      painter.scale(1,-1);
      painter.drawPixmap(QRectF(-data.playerRadius(),-data.playerRadius(),2*data.playerRadius(),data.playerRadius()),rightPlayerImage,rightPlayerImage.rect());
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
      painter.drawPixmap(QRectF(-data.netWidth()/2.,-data.netHeight(),data.netWidth(),data.netHeight()),poleImage,poleImage.rect());
      painter.restore();
  }

  if (debugdraw) { // debug draw scene
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


}

