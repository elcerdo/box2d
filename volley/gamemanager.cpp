#include "gamemanager.h"

static const float scene_height = 15;
static const float court_width = 20;
static const float court_height = 30;
static const float court_net_height = 3;
static const float court_net_width = 0.2;
static const float player_radius = 1.2;
static const float ball_radius = .5;
static const float tack_radius = .3;

float GameManager::courtWidth() { return court_width; }
float GameManager::courtHeight() { return court_height; }
float GameManager::netHeight() { return court_net_height; }
float GameManager::netWidth() { return court_net_width; }
float GameManager::ballRadius() { return ball_radius; }
float GameManager::playerRadius() { return player_radius; }
float GameManager::sceneHeight() { return scene_height; }
float GameManager::tackRadius() { return tack_radius; }

static const float player_move_speed = 8;
static const float player_jump_speed = 5.;
static const float g = 10;
static const float jump_factor = 3.;

float GameManager::playerMoveSpeed() { return player_move_speed; }
float GameManager::playerJumpSpeed() { return player_jump_speed; }
float GameManager::gravity() { return g; }
float GameManager::jumpFactor() { return jump_factor; }

