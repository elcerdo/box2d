#include "keymanager.h"

#include <Qt>

// default keys
static const int defaultFullscreenKey = Qt::Key_F;
static const int defaultDebugDrawKey = Qt::Key_T;
static const int defaultResetViewKey = Qt::Key_R;
static const int defaultBeginPointKey = Qt::Key_Space;

static const PlayerKeys defaultPlayerKeys[nplayers] = {
    PlayerKeys(Qt::Key_Q,Qt::Key_D,Qt::Key_Z),
    PlayerKeys(Qt::Key_Left,Qt::Key_Right,Qt::Key_Up)
};

PlayerKeys::PlayerKeys(int left,int right,int jump) : left(left), right(right), jump(jump) {}

int KeyManager::fullscreenKey() const { return defaultFullscreenKey; }
int KeyManager::debugDrawKey() const { return defaultDebugDrawKey; }
int KeyManager::resetViewKey() const { return defaultResetViewKey; }
int KeyManager::beginPointKey() const { return defaultBeginPointKey; }

PlayerKeys KeyManager::playerKeys(int number) const {
    Q_ASSERT(number>=0 && number<nplayers);
    return defaultPlayerKeys[number];
}

