#include "gamemanager.h"

static const float court_width = 20;
static const float court_net_height = 3;
static const float player_radius = 1.2;
static const float ball_radius = .75;

float GameManager::courtWidth() { return court_width; }
float GameManager::courtHeight() { return 12; }
float GameManager::netHeight() { return court_net_height; }
float GameManager::netWidth() { return 0.25; }
float GameManager::ballRadius() { return ball_radius; }
float GameManager::playerRadius() { return player_radius; }
float GameManager::sceneHeight() { return 15; }
float GameManager::tackRadius() { return .6; }

static const float player_move_speed = 8;
static const float player_jump_speed = 5.;
static const float g = 10;
static const float jump_factor = 3.5;

float GameManager::playerMoveSpeed() { return player_move_speed; }
float GameManager::playerJumpSpeed() { return player_jump_speed; }
float GameManager::gravity() { return g; }
float GameManager::jumpFactor() { return jump_factor; }

static const float ball_release_height = 6;

float GameManager::ballReleaseHeight() { return ball_release_height; }
float GameManager::startingTime() { return 1.5; }
float GameManager::groundLevel() { return 3; }
float GameManager::cursorHideTime() { return .5; }
