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
  panning(false), fullscreen(false), panningPosition(0,0), panningPositionStart(0,0), panningPositionCurrent(0,0), scale(0.), debugdraw(false),
  cursorMovedTime(0),
  data(data),
  ballImage(":/images/ball.png"), leftPlayerImage(":/images/left_blob.png"), rightPlayerImage(":/images/right_blob.png"),
  arrowImage(":/images/arrow.png"),
  trajectoryImage(200,150), birdImage(":/images/bird.png"),
  //win00(":/images/win00.png"),win01(":/images/win01.png"),
  //lose00(":/images/lose00.png"),lose01(":/images/lose01.png"),
  frame(":/images/tv_frame.png")
{
    resize(800,600);
    setCursor(QCursor(Qt::BlankCursor));
    setMouseTracking(true);
  
    { // load settings
	QSettings settings;
	scale = settings.value("drawer/scale",40.).toFloat();
	panningPosition = settings.value("drawer/panningPosition",0.).toPointF();
	debugdraw = settings.value("drawer/debugdraw",false).toBool();
	fullscreen = settings.value("drawer/fullscreen",false).toBool();
	if (fullscreen) setWindowState(windowState()|Qt::WindowFullScreen);
    }

    { // background noise
	noise_current = 0;
	QPixmapCache cache;
	for (int kk=0; kk<GameManager::numberOfNoiseBackground(); kk++) {
	    QString name = QString("noise%1").arg(kk);
	    bool ok = cache.find(name,&noises[kk]);
	    Q_ASSERT(ok);
	}
    }

    { // setup graphic stuff
	drawingPen.setColor("white");
	drawingPen.setWidthF(.25);
	drawingPen.setCapStyle(Qt::RoundCap);
	//drawingFont.setFamily("04b03");
	drawingFont.setFamily("PixelSplitter");
	drawingFont.setPixelSize(22);

	debugPen.setColor("red");
	debugFont.setBold(true);
	debugFont.setPixelSize(20);
    }

    time.start();
}

Drawer::~Drawer()
{
    QSettings settings;
    settings.setValue("drawer/scale",scale);
    settings.setValue("drawer/panningPosition",panningPosition);
    settings.setValue("drawer/debugdraw",debugdraw);
    settings.setValue("drawer/fullscreen",fullscreen);
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
    if (event->key()==KeyManager::exitKey()) {
      emit exitButtonPressed();
    }
    if (event->key()==KeyManager::fullscreenKey()) {
	fullscreen = !fullscreen;
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
    unsetCursor();
    if (world) cursorMovedTime = world->getTime();
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

void Drawer::drawPlayerName(const Player& player, QPainter &painter) const
{
    painter.scale(1,-1);
    painter.translate(0,1);
    painter.drawPixmap(QRectF(-.5,.5,1,1),arrowImage,arrowImage.rect());
    painter.translate(0,2);
    painter.scale(.05,-.05);
    painter.drawText(QRectF(-20,-15,40,30),Qt::AlignCenter,player.getName());
}

void Drawer::recordPhysicsStamp(World* world)
{
  // handle timestamp
  physics_stamps.push_front(time.elapsed());
  while (physics_stamps.size()>60) physics_stamps.pop_back();
}

void Drawer::paintEvent(QPaintEvent* event)
{
  if (!world) return;

  // handle timestamp
  frame_stamps.push_front(time.elapsed());
  while (frame_stamps.size()>30) frame_stamps.pop_back();

  if (world->getTime()>cursorMovedTime+GameManager::cursorHideTime()) setCursor(QCursor(Qt::BlankCursor));
 
  const float dt = world->getTime()-data.getLastTransitionTime();

  static QRectF message_position(-75,-130,150,20);

  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform,true);

  { // clear background
      //painter.beginNativePainting();
      //qglClearColor(Qt::black);
      //glClear(GL_COLOR_BUFFER_BIT);
      //painter.endNativePainting();
      painter.save();
      painter.setPen(Qt::NoPen);
      painter.setBrush(Qt::black);
      painter.drawRect(rect());
      painter.restore();
  }

  painter.setFont(drawingFont);
  painter.setPen(drawingPen);

  painter.translate(width()/2.,height());
  painter.translate(panningPosition);
  if (panning) painter.translate(panningPositionCurrent-panningPositionStart);
  painter.scale(scale,-scale);

  { // draw background
      data.getBall().drawPositions(trajectoryImage);

      painter.save();
      painter.scale(1,-1);
      QImage current = noises[noise_current].toImage();
      QPainter composition(&current);
      composition.setCompositionMode(QPainter::CompositionMode_Overlay);
      composition.drawPixmap(0,0,trajectoryImage);
      composition.end();
      painter.drawImage(QRectF(-GameManager::courtWidth()/2,-GameManager::sceneHeight(),GameManager::courtWidth(),GameManager::sceneHeight()),current,current.rect());
      //painter.drawPixmap(QRectF(-GameManager::courtWidth()/2,-GameManager::sceneHeight(),GameManager::courtWidth(),GameManager::sceneHeight()),trajectoryImage,trajectoryImage.rect());
      painter.restore();
      noise_current++;
      noise_current%=GameManager::numberOfNoiseBackground();
  }

  { // draw score
      painter.save();
      painter.scale(.1,-.1);
      //painter.drawRect(QRectF(-32,-30,30,25));
      painter.drawText(QRectF(-34.5,-30,30,25),Qt::AlignRight|Qt::AlignTop,QString("%1").arg(data.getTeam(Team::LEFT).getScore()));
      //painter.drawRect(QRectF(2,-30,30,25));
      painter.drawText(QRectF(4,-30,30,25),Qt::AlignLeft|Qt::AlignTop,QString("%1").arg(data.getTeam(Team::RIGHT).getScore()));
      painter.restore();
  }

  { // draw sets
      painter.save();
      painter.setPen(Qt::NoPen);
      painter.setBrush(drawingPen.color());

      painter.save();
      painter.translate(-GameManager::courtWidth()/2,2);
      for (int kk=0; kk<data.getTeam(Team::LEFT).getSet() && kk<4; kk++) {
	  painter.translate(1.2,0);
	  painter.drawEllipse(QRectF(-.5,-.5,1,1));
      }
      painter.restore();

      painter.save();
      painter.translate(GameManager::courtWidth()/2,2);
      for (int kk=0; kk<data.getTeam(Team::RIGHT).getSet() && kk<4; kk++) {
	  painter.translate(-1.2,0);
	  painter.drawEllipse(QRectF(-.5,-.5,1,1));
      }
      painter.restore();

      painter.restore();
  }

  if (data.getState()==GameData::STARTING) { // draw ready overlay
      painter.save();
      painter.scale(.1,-.1);
      painter.drawText(message_position,Qt::AlignCenter,"READY");
      painter.restore();
  }

  if (data.getState()==GameData::PLAYING && dt<1. && (dt-static_cast<int>(dt/.25)*.25<.25/2.)) { // draw go overlay
      painter.save();
      painter.scale(.1,-.1);
      painter.drawText(message_position,Qt::AlignCenter,"GO");
      painter.restore();
  }

  if (data.getState()==GameData::FINISHED) { // draw ready overlay
      bool aa = (dt-static_cast<int>(dt/.5)*.5<.5/2.);

      const Team& team = data.getLastScoringTeam();
      const Player* player = data.getLastScoringPlayer();

      QString message = QString("%1 SCORED").arg(team.getName());
      if (player) {
	  if (player->getTeam().getField()==team.getField()) message = QString("%1 SCORED").arg(player->getName());
	  else message = QString("%1 FAILED").arg(player->getName());
      }

      painter.save();
      painter.scale(.1,-.1);
      painter.drawText(message_position,Qt::AlignCenter,message); // finished
      if (aa) { // press space
	  QFont font(drawingFont);
	  font.setPixelSize(drawingFont.pixelSize()/2);
	  painter.setFont(font);
	  QRectF rectangle(message_position);
	  rectangle.setTop(rectangle.top()+25);
	  rectangle.setBottom(rectangle.bottom()+25);
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
      //if (data.isLastTouchingPlayer(player)) painter.drawRect(QRectF(-GameManager::playerRadius(),-GameManager::playerRadius(),2*GameManager::playerRadius(),GameManager::playerRadius()));
      //if (data.getState()==GameData::STARTING || (data.getState()==GameData::PLAYING && dt<1. && (dt-static_cast<int>(dt/.25)*.25<.25/2.))) drawPlayerName(player,painter);
      if (data.getState()==GameData::STARTING && dt<1. && (dt-static_cast<int>(dt/.25)*.25<.25/2.)) drawPlayerName(player,painter);
      //if (data.getState()==GameData::STARTING) drawPlayerName(player,painter);
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
      //if (data.isLastTouchingPlayer(player)) painter.drawRect(QRectF(-GameManager::playerRadius(),-GameManager::playerRadius(),2*GameManager::playerRadius(),GameManager::playerRadius()));
      //if (data.getState()==GameData::STARTING || (data.getState()==GameData::PLAYING && dt<1. && (dt-static_cast<int>(dt/.25)*.25<.25/2.))) drawPlayerName(player,painter);
      if (data.getState()==GameData::STARTING && dt<1. && (dt-static_cast<int>(dt/.25)*.25<.25/2.)) drawPlayerName(player,painter);
      //if (data.getState()==GameData::STARTING) drawPlayerName(player,painter);
      painter.restore();
  }

  /*{ // draw bird
      const Bird& player = data.getBird();
      const b2Body* body = player.getBody();

      painter.save();
      painter.translate(0,GameManager::groundLevel());
      painter.translate(toQPointF(body->GetPosition()));
      painter.scale(1,-1);
      painter.rotate(-body->GetAngle()*180/b2_pi);
      painter.drawPixmap(QRectF(-GameManager::birdSize(),-GameManager::birdSize(),2*GameManager::birdSize(),2*GameManager::birdSize()),birdImage,birdImage.rect());
      painter.restore();
  }*/

  { // draw ground and net
      painter.save();
      painter.translate(0,GameManager::groundLevel());
      painter.drawLine(QPointF(-GameManager::courtWidth()/2,-drawingPen.widthF()/2),QPointF(GameManager::courtWidth()/2,-drawingPen.widthF()/2));
      painter.drawLine(QPointF(0,0),QPointF(0,GameManager::netHeight()-GameManager::netWidth()/3));
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
      }

      float frame_fps = 0;
      float physics_fps = 0;
      if (!frame_stamps.empty()) frame_fps = 1000.*(frame_stamps.size()-1)/(frame_stamps.front()-frame_stamps.back());
      if (!physics_stamps.empty()) physics_fps = 1000.*(physics_stamps.size()-1)/(physics_stamps.front()-physics_stamps.back());

      painter.save();
      painter.resetTransform();
      painter.setFont(debugFont);
      painter.setPen(debugPen);
      painter.drawText(5,20,"state " + state_string);
      painter.drawText(5,40,QString("transition %1s").arg(dt,0,'f',2));
      painter.drawText(5,60,QString("fps %1 pps %2").arg(frame_fps,0,'f',2).arg(physics_fps,0,'f',2));
      painter.restore();
  }


  if (debugdraw) { // debug draw scene
      painter.save();
      painter.setFont(debugFont);
      painter.setPen(debugPen);
      painter.translate(0,GameManager::groundLevel());

      // draw bodies
      for (const b2Body* body=world->getFirstBody(); body!=NULL; body=body->GetNext()) {
	  painter.save();
	  painter.translate(toQPointF(body->GetPosition()));
	  painter.rotate(body->GetAngle()*180/b2_pi);

	  if (body->IsAwake()) painter.setBrush(QBrush(QColor::fromRgbF(1,1,0,.3)));
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

      //{ // draw joints
      //    painter.save();
      //    painter.setPen(QPen(QColor::fromRgbF(0,1,0)));
      //    for (const b2Joint* joint=world->getFirstJoint(); joint!=NULL; joint=joint->GetNext()) {
      //        painter.drawLine(toQPointF(joint->GetAnchorA()),toQPointF(joint->GetAnchorB()));
      //    }
      //    painter.restore();
      //}

      painter.restore();
  }


}

