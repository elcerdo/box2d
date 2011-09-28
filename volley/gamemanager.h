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

    static float ballReleaseHeight();
    static float startingTime();

    static float groundLevel();
    static float cursorHideTime();
    static int numberOfPositions();
    static int winningScore();

    static int numberOfNoiseBackground();
private:
    GameManager(); // not implemented
};

#endif
