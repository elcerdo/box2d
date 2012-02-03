#include "drawer.h"
#include <QPainter>
#include <QColor>
#include <QSettings>
#include <QPixmapCache>
#include "common.h"

QPointF toQPointF(const b2Vec2 &vect)
{
  return QPointF(vect.x,vect.y);
}

//scale is in pixel/m

Drawer::Drawer(GameState& state, QWidget *parent)
//: QGLWidget(parent),
: QWidget(parent),
  world(NULL),
  panning(false), fullscreen(false), panningPosition(0,0), panningPositionStart(0,0), panningPositionCurrent(0,0), scale(0.), debugdraw(false),
  cursorMovedTime(0),
  state(state)
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

	{ // setup pens and brushes
		debugPen.setColor(Qt::red);
		debugFont.setBold(true);
		debugFont.setPixelSize(18);
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

    if (event->key() == fullscreenKey) {
		fullscreen = !fullscreen;
		setWindowState(windowState() ^ Qt::WindowFullScreen);
		event->accept();
		return;
    }

    if (event->key() == resetKey) {
		panningPosition = QPointF(0,0);
		const float scaleWidth = width()/boardSize;
		const float scaleHeight = height()/boardSize;
		scale = scaleWidth<scaleHeight ? scaleWidth : scaleHeight;
		update();
		event->accept();
		return;
    }

    if (event->key() == debugKey) {
		debugdraw = !debugdraw;
		update();
		event->accept();
		return;
    }

    if (event->key() == Qt::Key_Space) {
		state.hitBall();
		update();
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

    event->ignore();
}

void Drawer::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == panningButton) {
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
	if (event->button() == panningButton) {
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

	// handle timestamp
	frame_stamps.push_front(time.elapsed());
	while (frame_stamps.size()>30) frame_stamps.pop_back();

	if (world->getTime() > cursorMovedTime+cursorHideTime) setCursor(QCursor(Qt::BlankCursor));

	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform,true);

	{ // clear background
		//painter.beginNativePainting();
		//qglClearColor(Qt::black);
		//glClear(GL_COLOR_BUFFER_BIT);
		//painter.endNativePainting();
		painter.save();
		painter.setPen(Qt::NoPen);
		painter.setBrush(Qt::white);
		painter.drawRect(rect());
		painter.restore();
	}

	painter.translate(width()/2.,height()/2.);
	painter.translate(panningPosition);
	if (panning) painter.translate(panningPositionCurrent-panningPositionStart);
	painter.scale(scale,-scale);

	if (debugdraw) { // draw state overlay
		QString state_string = "unknown";

		float frame_fps = 0;
		if (!frame_stamps.empty()) frame_fps = 1000.*(frame_stamps.size()-1)/(frame_stamps.front()-frame_stamps.back());

		painter.save();
		painter.setPen(debugPen);
		painter.setFont(debugFont);
		painter.resetTransform();
		painter.drawText(5,20,"state " + state_string);
		painter.drawText(5,40,QString("fps %1").arg(frame_fps,0,'f',2));
		painter.drawText(5,60,QString("%1 bodies").arg(world->getBodyCount()));
		painter.restore();
	}

	{
		painter.save();
		painter.setPen(Qt::red);
		painter.drawLine(0,0,1,0);
		painter.setPen(Qt::green);
		painter.drawLine(0,0,0,1);
		painter.restore();
	}


	if (debugdraw) { // debug draw scene
		painter.save();
		painter.setPen(debugPen);
		painter.setFont(debugFont);

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

