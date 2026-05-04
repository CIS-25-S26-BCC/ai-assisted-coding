#include "player.h"

#include <cmath>

namespace {

constexpr float kMoveSpeed = 8.0f;
constexpr float kGravity = 24.0f;
constexpr float kJumpVelocity = -10.0f;

bool isSolid(const Map& map, float x, float y) {
  const int col = static_cast<int>(std::floor(x));
  const int row = static_cast<int>(std::floor(y));
  return map.tileAt(col, row) == TileType::Solid;
}

bool isHazard(const Map& map, float x, float y) {
  const int col = static_cast<int>(std::floor(x));
  const int row = static_cast<int>(std::floor(y));
  return map.tileAt(col, row) == TileType::Hazard;
}

} // namespace

Player::Player(float startX, float startY)
    : x(startX), y(startY),
      _velX(0.0f), _velY(0.0f),
      _spawnX(startX), _spawnY(startY),
    _dead(false), _grounded(false) {}

void Player::applyInput(char key) {
  if (key == 'a' || key == 'A') {
    _velX = -kMoveSpeed;
  } else if (key == 'd' || key == 'D') {
    _velX = kMoveSpeed;
  } else if (key == 'w' || key == 'W' || key == ' ') {
    if (_grounded) {
      _velY = kJumpVelocity;
      _grounded = false;
    }
  }
}

void Player::update(float dt, const Map& map) {
  if (_dead || dt <= 0.0f) {
    return;
  }

  _velY += kGravity * dt;

  const float nextX = x + (_velX * dt);
  if (!isSolid(map, nextX, y)) {
    x = nextX;
  } else {
    _velX = 0.0f;
  }

  const float nextY = y + (_velY * dt);
  if (!isSolid(map, x, nextY)) {
    y = nextY;
    _grounded = false;
  } else {
    if (_velY > 0.0f) {
      _grounded = true;
      y = std::floor(y);
    }
    _velY = 0.0f;
  }

  if (isHazard(map, x, y)) {
    _dead = true;
  }

  // Small drag to keep controls simple and avoid perpetual sliding.
  _velX *= 0.75f;
  if (std::fabs(_velX) < 0.05f) {
    _velX = 0.0f;
  }
}

bool Player::isDead() const { return _dead; }

void Player::respawn() {
  x = _spawnX;
  y = _spawnY;
  _velX = 0.0f;
  _velY = 0.0f;
  _dead = false;
  _grounded = false;
}
