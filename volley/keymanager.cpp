#include "keymanager.h"

#include <QDebug>
#include <Qt>

// default keys
static const int defaultFullscreenKey = Qt::Key_F;
static const int defaultDebugDrawKey = Qt::Key_T;
static const int defaultResetViewKey = Qt::Key_R;
static const int defaultBeginPointKey = Qt::Key_Space;
static const int defaultExitKey = Qt::Key_Escape;


static const PlayerKeys defaultPlayerKeys[nplayers] = {
    PlayerKeys(Qt::Key_Q,Qt::Key_D,Qt::Key_Z),
    PlayerKeys(Qt::Key_Left,Qt::Key_Right,Qt::Key_Up)
};

PlayerKeys::PlayerKeys(int left,int right,int jump) : left(left), right(right), jump(jump) {}

int KeyManager::fullscreenKey() { return defaultFullscreenKey; }
int KeyManager::debugDrawKey() { return defaultDebugDrawKey; }
int KeyManager::resetViewKey() { return defaultResetViewKey; }
int KeyManager::beginPointKey() { return defaultBeginPointKey; }
int KeyManager::exitKey() {return defaultExitKey; }

PlayerKeys KeyManager::playerKeys(int number) {
    Q_ASSERT(number>=0 && number<nplayers);
    return defaultPlayerKeys[number];
}

void KeyManager::dumpKeys() {
    qDebug() << "[GENERAL]";
    qDebug() << "fullscreen" << keyToString(fullscreenKey());
    qDebug() << "debugdraw" << keyToString(debugDrawKey());
    qDebug() << "resetview" << keyToString(resetViewKey());
    qDebug() << "beginpoint" << keyToString(beginPointKey());
    qDebug() << "Exit" << keyToString(exitKey());
    for (int kk=0; kk<2; kk++) {
	PlayerKeys keys = playerKeys(kk);
	qDebug() << qPrintable(QString("[PLAYER%1]").arg(kk));
	qDebug() << "left" << keyToString(keys.left);
	qDebug() << "right" << keyToString(keys.right);
	qDebug() << "jump" << keyToString(keys.jump);
    }
}
