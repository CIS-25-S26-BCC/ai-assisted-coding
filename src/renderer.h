#pragma once

#include "map.h"
#include "player.h"
#include "types.h"

// Renderer is a stateless helper that owns all terminal output.
// It is the only class that writes to stdout.
// main() calls draw() once per frame, after Player::update() has finished,
// so it always reads a fully-resolved position for the current frame.
class Renderer {
public:
    // Clears the terminal and redraws the entire scene for one frame.
    // Rendering order:
    //   1. Move cursor to top-left (ANSI escape: \033[H).
    //   2. Iterate every (col, row) in the map via Map::tileAt(); print the
    //      corresponding ASCII character ('#' solid, '.' empty, 'X' hazard, 'G' goal).
    //   3. Overwrite the cell at (player.x, player.y) with '@' to show the player.
    //   4. Print a HUD line below the map showing controls and the current GameState
    //      (e.g. "[A/D] Move  [W/Space] Jump  [Q] Quit  | Status: Running").
    // All output is written in a single pass to minimise flicker.
    void draw(const Map& map, const Player& player, GameState state);
};
