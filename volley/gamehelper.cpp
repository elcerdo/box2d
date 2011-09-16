#include "gamehelper.h"
#include "gamemanager.h"

#include <QPainter>
#include <QDebug>

UserData::UserData(b2Body* body) : body(body) { Q_ASSERT(body); body->SetUserData(static_cast<void*>(this)); }
const b2Body* UserData::getBody() const { return body; }

Ball::Ball(b2Body* body) : UserData(body), nhit(0) {}

void Ball::recordPosition()
{
    //qDebug() << "record" << positions.size();
    Position position = {body->GetPosition().x,body->GetPosition().y};
    positions.push_back(position);
    while (positions.size()>GameManager::numberOfPositions()) { positions.pop_front(); }
}

void Ball::clearPositions()
{
    positions.clear();
}

void Ball::drawPositions(QPixmap& pixmap) const
{
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(Qt::NoPen);
    painter.translate(pixmap.width()/2.,pixmap.height());
    painter.scale(pixmap.width()/GameManager::courtWidth(),-pixmap.width()/GameManager::courtWidth());
    painter.translate(-GameManager::ballRadius(),GameManager::groundLevel()-GameManager::ballRadius());
    int kk=0;
    for (Positions::const_iterator iter=positions.begin(); iter!=positions.end(); iter++) {
	painter.setBrush(QColor::fromHsv(kk,255,255-kk));
	painter.drawEllipse(QRectF(iter->x,iter->y,2*GameManager::ballRadius(),2*GameManager::ballRadius()));
	kk+=17;
	kk%=360;
    }
}


Team::Team(const Team::Field &field) : field(field), score(0) {}
Team::Field Team::getField() const { return field; }
int Team::getScore() const { return score; }
void Team::teamScored() { score++; }

Player::Player(b2Body* body, Team &team) : UserData(body), team(team), jumping(false), jump_speed(0), jump_time(-1) {}

void Player::goLeft(float /*time*/)
{
    body->SetLinearVelocity(b2Vec2(-GameManager::playerMoveSpeed(),body->GetLinearVelocity().y));
}

void Player::goRight(float /*time*/)
{
    body->SetLinearVelocity(b2Vec2(GameManager::playerMoveSpeed(),body->GetLinearVelocity().y));
}

void Player::stopLeft(float /*time*/)
{
    if (body->GetLinearVelocity().x<0) body->SetLinearVelocity(b2Vec2(0,body->GetLinearVelocity().y));
}

void Player::stopRight(float /*time*/)
{
    if (body->GetLinearVelocity().x>0) body->SetLinearVelocity(b2Vec2(0,body->GetLinearVelocity().y));
}

void Player::goJump(float time)
{
    if (jumping) return;
    jumping = true;
    jump_time = time;
    jump_speed = GameManager::playerJumpSpeed();

    body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, GameManager::jumpFactor()*jump_speed));
}

void Player::stopJump(float time)
{
    const float current_speed = GameManager::gravity()*(time-jump_time);
    if (jump_speed - current_speed > 0)
	jump_speed = current_speed;
}

void Player::checkJump(float time)
{
    if (!jumping) return;

    if (body->GetPosition().y < .05){
	body->SetTransform(b2Vec2(body->GetPosition().x, 0),0);
	body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, 0));
	jumping = false;
	return;
    }

    body->SetLinearVelocity(b2Vec2(body->GetLinearVelocity().x, GameManager::jumpFactor()*(jump_speed - GameManager::gravity()*(time-jump_time))));
}
    
void Player::checkPosition(float time)
{
    const float player_x = body->GetPosition().x;
    const float player_y = body->GetPosition().y;
    const float player_vy = body->GetLinearVelocity().y;

    float player_x_min = 0;
    float player_x_max = 0;
    switch (team.getField()) {
    case Team::LEFT:
	player_x_min = -GameManager::courtWidth()/2.+GameManager::playerRadius();
	player_x_max = -GameManager::netWidth()/2.-GameManager::playerRadius();
	break;
    case Team::RIGHT:
	player_x_max = GameManager::courtWidth()/2.-GameManager::playerRadius();
	player_x_min = GameManager::netWidth()/2.+GameManager::playerRadius();
	break;
    }

    if (player_x<player_x_min) {
	body->SetTransform(b2Vec2(player_x_min,player_y),0);
	body->SetLinearVelocity(b2Vec2(0,player_vy));
    }
    if (player_x>player_x_max) {
	body->SetTransform(b2Vec2(player_x_max,player_y),0);
	body->SetLinearVelocity(b2Vec2(0,player_vy));
    }
}

Team &Player::getTeam() { return team; }
