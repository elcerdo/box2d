#ifndef __KEYMANAGER_H__
#define __KEYMANAGER_H__

#include "common.h"

struct PlayerKeys {
    PlayerKeys(int left,int right,int jump);
    const int left;
    const int right;
    const int jump;
};

class KeyManager {
public:
    static int fullscreenKey();
    static int debugDrawKey();
    static int resetViewKey();
    static int beginPointKey();
    static int exitKey();
    static PlayerKeys playerKeys(int number);

    static void dumpKeys();
private:
    KeyManager(); // not implemented
};

#endif

