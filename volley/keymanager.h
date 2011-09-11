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

    int fullscreenKey() const;
    int debugDrawKey() const;
    int resetViewKey() const;
    int beginPointKey() const;

    PlayerKeys playerKeys(int number) const;

    template <class Stream> 
    void dumpKeys(Stream &stream) const {
	stream << "[GENERAL]";
	stream << "fullscreen" << keyToString(fullscreenKey());
	stream << "debugdraw" << keyToString(debugDrawKey());
	stream << "resetview" << keyToString(resetViewKey());
	stream << "beginpoint" << keyToString(beginPointKey());
	for (int kk=0; kk<2; kk++) {
	    PlayerKeys keys = playerKeys(kk);
	    stream << qPrintable(QString("[PLAYER%1]").arg(kk));
	    stream << "left" << keyToString(keys.left);
	    stream << "right" << keyToString(keys.right);
	    stream << "jump" << keyToString(keys.jump);
	}
    }
};

#endif

