#include "gamemanager.h"
#include "drawer.h"
#include "common.h"
#include <QPainter>
#include <QColor>
#include <QSettings>
#include <QPixmapCache>

bool Drawer::handlePlayerKeyPress(int key, const PlayerKeys& keys, Player& player, float time)
{
    if (key==keys.left) { player.goLeft(time); return true; }
    if (key==keys.right) { player.goRight(time); return true; }
    if (key==keys.jump) { player.goJump(time); return true; }
    return false;
}

bool Drawer::handlePlayerKeyRelease(int key, const PlayerKeys& keys, Player& player, float time)
{
    if (key==keys.left) { player.stopLeft(time); return true; }
    if (key==keys.right) { player.stopRight(time); return true; }
    if (key==keys.jump) { player.stopJump(time); return true; }
    return false;
}

static const Qt::MouseButton panningButton = Qt::MidButton;

//scale is in pixel/m

Drawer::Drawer(GameData& data,QWidget *parent)
: QGLWidget(parent), world(NULL),
  panning(false), panningPosition(0,0), panningPositionStart(0,0), panningPositionCurrent(0,0), scale(0.), debugdraw(false),
  data(data),
  ballImage(":/images/ball.png"), leftPlayerImage(":/images/left_blob.png"), rightPlayerImage(":/images/right_blob.png"),
  arrowImage(":/images/arrow.png"),
  //win00(":/images/win00.png"),win01(":/images/win01.png"),
  //lose00(":/images/lose00.png"),lose01(":/images/lose01.png"),
  frame(":/images/tv_frame.png")
{
    resize(800,600);
  
    QSettings settings;
    scale = settings.value("drawer/scale",40.).toFloat();
    panningPosition = settings.value("drawer/panningPosition",0.).toPointF();
    debugdraw = settings.value("drawer/debugdraw",false).toBool();

    { // background noise
	noise_current = 0;
	QPixmapCache cache;
	for (int kk=0; kk<10; kk++) {
	    QString name = QString("noise%1").arg(kk);
	    bool ok = cache.find(name,&noises[kk]);
	    Q_ASSERT(ok);
	}
    }
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

    if (event->key()==KeyManager::fullscreenKey()) {
	setWindowState(windowState() ^ Qt::WindowFullScreen);
	event->accept();
	return;
    }

    if (event->key()==KeyManager::resetViewKey()) {
	panningPosition = QPointF(0,0);
	const float scaleWidth = width()/GameManager::courtWidth();
	const float scaleHeight = height()/GameManager::sceneHeight();
	scale = scaleWidth<scaleHeight ? scaleWidth : scaleHeight;
	update();
	event->accept();
	return;
    }

    if (event->key()==KeyManager::debugDrawKey()) {
	debugdraw = !debugdraw;
	update();
	event->accept();
	return;
    }

    if (data.getState()==GameData::FINISHED && event->key()==KeyManager::beginPointKey()) {
	data.beginPoint(*world);
	event->accept();
	return;
    }

    for (int kk=0; kk<nplayers; kk++) {
	if (handlePlayerKeyPress(event->key(),KeyManager::playerKeys(kk),data.getPlayer(kk),world->getTime())) {
	    event->accept();
	    return;
	}
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

    for (int kk=0; kk<nplayers; kk++) {
	if (handlePlayerKeyRelease(event->key(),KeyManager::playerKeys(kk),data.getPlayer(kk),world->getTime())) {
	    event->accept();
	    return;
	}
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
 
  const float dt = world->getTime()-data.getLastTransitionTime();

  static QPen drawing_pen;
  drawing_pen.setColor("white");
  drawing_pen.setWidthF(.25);
  drawing_pen.setCapStyle(Qt::FlatCap);
  static QFont drawing_font("04b03");
  drawing_font.setPixelSize(100);

  QRectF message_position(width()/2.-300,120,600,100);

  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform,true);
  painter.setFont(drawing_font);
  painter.setPen(drawing_pen);

  painter.translate(width()/2.,height());
  painter.translate(panningPosition);
  if (panning) painter.translate(panningPositionCurrent-panningPositionStart);
  painter.scale(scale,-scale);

  { // draw background
      painter.save();
      painter.scale(1,-1);
      painter.drawPixmap(QRectF(-GameManager::courtWidth()/2,-GameManager::sceneHeight(),GameManager::courtWidth(),GameManager::sceneHeight()),noises[noise_current],noises[noise_current].rect());
      painter.restore();
      noise_current++;
      noise_current%=10;
  }

  { // draw score
      painter.save();
      painter.resetTransform();
      painter.translate(width()/2,height());
      painter.drawText(QRect(-160,-115,150,100),Qt::AlignRight,QString("%1").arg(data.getTeam(Team::LEFT).getScore()));
      painter.drawText(QRect(20,-115,150,100),Qt::AlignLeft,QString("%1").arg(data.getTeam(Team::RIGHT).getScore()));
      painter.restore();
  }

  if (data.getState()==GameData::STARTING) { // draw ready overlay
      painter.save();
      painter.resetTransform();
      painter.drawText(message_position,Qt::AlignCenter,"READY");
      painter.restore();
  }

  if (data.getState()==GameData::PLAYING && dt<1. && (dt-static_cast<int>(dt/.25)*.25<.25/2.)) { // draw ready overlay
      painter.save();
      painter.resetTransform();
      painter.drawText(message_position,Qt::AlignCenter,"GO");
      painter.restore();
  }

  if (data.getState()==GameData::FINISHED) { // draw ready overlay
      bool aa = (dt-static_cast<int>(dt/.5)*.5<.5/2.);
      painter.save();
      painter.resetTransform();
      painter.drawText(message_position,Qt::AlignCenter,"FINISHED"); // finished
      if (aa) { // press space
	  QFont font(drawing_font);
	  font.setPixelSize(30);
	  painter.setFont(font);
	  QRectF rectangle(message_position);
	  rectangle.setTop(rectangle.top()+80);
	  rectangle.setBottom(rectangle.bottom()+80);
	  painter.drawText(rectangle,Qt::AlignCenter,"press space");
      }
      //{ // win animation
      //    painter.save();
      //    if (data.getLastScoringTeam().getField()==Team::LEFT) painter.translate(width()/4.,300);
      //    else painter.translate(3.*width()/4.,300);
      //    QRect rectangle(-50,-50,100,100);
      //    if (aa) painter.drawPixmap(rectangle,win00,win00.rect());
      //    else painter.drawPixmap(rectangle,win01,win01.rect());
      //    painter.restore();
      //}
      //{ // lose animation
      //    painter.save();
      //    if (data.getLastScoringTeam().getField()==Team::RIGHT) painter.translate(width()/4.,300);
      //    else painter.translate(3.*width()/4.,300);
      //    QRect rectangle(-50,-50,100,100);
      //    if (aa) painter.drawPixmap(rectangle,lose00,lose00.rect());
      //    else painter.drawPixmap(rectangle,lose01,lose01.rect());
      //    painter.restore();
      //}
      painter.restore();
  }

  { // draw ball
      const Ball& ball = data.getBall();
      const b2Body* body = ball.getBody();

      painter.save();
      painter.translate(0,GameManager::groundLevel());
      painter.translate(toQPointF(body->GetPosition()));
      painter.scale(1,-1);
      painter.rotate(-body->GetAngle()*180/b2_pi);
      painter.drawPixmap(QRectF(-GameManager::ballRadius(),-GameManager::ballRadius(),2*GameManager::ballRadius(),2*GameManager::ballRadius()),ballImage,ballImage.rect());
      painter.restore();


      if (body->GetPosition().y>GameManager::sceneHeight()+GameManager::ballRadius()) {
	  float mini_ball_size = 1.-(body->GetPosition().y-GameManager::sceneHeight()-GameManager::ballRadius())/15;
	  if (mini_ball_size<.2) mini_ball_size = .2;

	  painter.save();
	  painter.translate(QPointF(body->GetPosition().x,GameManager::sceneHeight()-.6));
	  painter.scale(1,-1);
	  painter.drawPixmap(QRectF(-.4,-.4,.8,.8),arrowImage,arrowImage.rect());
	  painter.translate(0,1);
	  painter.rotate(-body->GetAngle()*180/b2_pi);
	  painter.drawPixmap(QRectF(-mini_ball_size/2.,-mini_ball_size/2.,mini_ball_size,mini_ball_size),ballImage,ballImage.rect());
	  painter.restore();
      }
  }

  { // draw left player
      const Player& player = data.getLeftPlayer();
      const b2Body* body = player.getBody();

      painter.save();
      painter.translate(0,GameManager::groundLevel());
      painter.translate(toQPointF(body->GetPosition()));
      painter.scale(1,-1);
      painter.drawPixmap(QRectF(-GameManager::playerRadius(),-GameManager::playerRadius(),2*GameManager::playerRadius(),GameManager::playerRadius()),leftPlayerImage,leftPlayerImage.rect());
      painter.restore();
  }

  { // draw right player
      const Player& player = data.getRightPlayer();
      const b2Body* body = player.getBody();

      painter.save();
      painter.translate(0,GameManager::groundLevel());
      painter.translate(toQPointF(body->GetPosition()));
      painter.scale(1,-1);
      painter.drawPixmap(QRectF(-GameManager::playerRadius(),-GameManager::playerRadius(),2*GameManager::playerRadius(),GameManager::playerRadius()),rightPlayerImage,rightPlayerImage.rect());
      painter.restore();
  }

  { // draw ground and net
      painter.save();
      painter.translate(0,GameManager::groundLevel());
      painter.setPen(drawing_pen);
      painter.drawLine(QPointF(-GameManager::courtWidth()/2,-drawing_pen.widthF()/2),QPointF(GameManager::courtWidth()/2,-drawing_pen.widthF()/2));
      painter.drawLine(QPointF(0,0),QPointF(0,GameManager::netHeight()));
      painter.restore();
  }

  { // draw tv frame
      painter.save();
      painter.translate(0,GameManager::sceneHeight()/2);
      painter.scale(1.04,-1.04);
      painter.drawPixmap(QRectF(-GameManager::courtWidth()/2,-GameManager::sceneHeight()/2,GameManager::courtWidth(),GameManager::sceneHeight()),frame,frame.rect());
      painter.restore();
  }

  if (debugdraw) { // draw state overlay
      QString state_string;
      switch (data.getState()) {
	  case GameData::INIT:
	      state_string = "init";
	      break;
	  case GameData::STARTING:
	      state_string = QString("starting");
	      break;
	  case GameData::PLAYING:
	      state_string = "playing";
	      break;
	  case GameData::FINISHED:
	      state_string = "finished";
	      break;
      };

      painter.save();
      painter.resetTransform();
      QFont font;
      font.setBold(true);
      font.setPixelSize(20);
      painter.setFont(font);
      painter.setPen(QColor("red"));
      painter.drawText(5,20,state_string);
      painter.drawText(5,40,QString("%1s").arg(dt,0,'f',2));
      painter.restore();
  }


  if (debugdraw) { // debug draw scene
      // draw bodies
      for (const b2Body* body=world->getFirstBody(); body!=NULL; body=body->GetNext()) {
	  painter.save();
	  painter.translate(0,GameManager::groundLevel());
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

