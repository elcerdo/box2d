#ifndef __GAMEMANAGER_H__
#define __GAMEMANAGER_H__

class GameManager {
public:
    static float sceneHeight();
    static float courtWidth();
    static float courtHeight();
    static float netHeight();
    static float netWidth();
    static float ballRadius();
    static float playerRadius();
    static float tackRadius();

    static float playerMoveSpeed();
    static float playerJumpSpeed();
    static float gravity();
    static float jumpFactor();
private:
    GameManager(); // not implemented
};

#endif
