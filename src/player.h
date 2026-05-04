#pragma once

#include "map.h"

// Player owns the character's position, velocity, and alive/dead status.
// Each frame in main(), three things happen in order:
//   1. applyInput(key)  — translate the raw keypress into a velocity intent
//   2. update(dt, map)  — advance physics, resolve tile collisions, detect hazard/goal
//   3. Renderer::draw() — read x/y to know where to draw the '@' character
class Player {
public:
    // Current position in tile-space (e.g. x=3.5 means halfway into column 3).
    // Public so Renderer can read them directly without a getter.
    float x;
    float y;

    // Initialises the player at (startX, startY) with zero velocity.
    // startX and startY come from Map::spawnPosition(), converted to float.
    // Also records these as the respawn coordinates for future calls to respawn().
    Player(float startX, float startY);

    // Translates a raw character from stdin into a change in _velX or a jump.
    //   'a' / 'A' — accelerate left (_velX negative)
    //   'd' / 'D' — accelerate right (_velX positive)
    //   'w' / 'W' / ' ' — initiate a jump if _grounded is true (sets _velY upward)
    //   'q' / 'Q' — handled in main() before this is called; ignored here
    // This does NOT move the player — it only sets velocity intent.
    // Actual position change happens in update().
    void applyInput(char key);

    // Advances the player's physics simulation by dt seconds and resolves collisions.
    // Called every frame after applyInput().
    // Steps performed (implementation details, deferred to next session):
    //   1. Apply gravity to _velY.
    //   2. Move x by _velX * dt; resolve horizontal tile collisions via Map::tileAt().
    //   3. Move y by _velY * dt; resolve vertical tile collisions; set _grounded flag.
    //   4. If the occupied tile is TileType::Hazard, set _dead = true.
    //   5. If the occupied tile is TileType::Goal, main() will read GameState::Won
    //      (the state transition itself lives in main(), not here).
    void update(float dt, const Map& map);

    // Returns true if the player has touched a Hazard tile this frame.
    // main() checks this after update(); if true, it sets GameState::Dead
    // and eventually calls respawn() after a brief pause.
    bool isDead() const;

    // Resets the player to the spawn position with zero velocity and clears _dead.
    // Called by main() after the GameState::Dead pause has elapsed.
    void respawn();

private:
    float _velX;   // horizontal velocity in tiles/second
    float _velY;   // vertical velocity in tiles/second (positive = downward)
    float _spawnX; // x coordinate to return to on respawn
    float _spawnY; // y coordinate to return to on respawn
    bool _dead;    // true for one frame after touching a Hazard tile
    bool _grounded; // true when the player is standing on a Solid tile below them
};
