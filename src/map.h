#pragma once

#include <string>
#include <vector>
#include "types.h"

// Map holds the static level layout as a 2-D grid of TileTypes.
// It is constructed once in main() from an embedded string literal and then
// passed by const reference to Player::update() and Renderer::draw().
// Map itself never changes during a play session — it has no mutable state.
class Map {
public:
    // Parses a multi-line string literal into rows of tiles.
    // Each character in the string maps to a TileType (e.g. '#' → Solid,
    // '.' → Empty, 'X' → Hazard, 'G' → Goal, 'S' → Spawn).
    // The exact character mapping is defined in the implementation.
    explicit Map(const std::string& layout);

    // Returns the TileType at grid position (col, row).
    // col is the horizontal index (0 = left), row is the vertical index (0 = top).
    // Player::update() calls this to check the four tiles surrounding the player
    // position each frame for collision, hazard, and goal detection.
    // Returns TileType::Solid for out-of-bounds coordinates as a safety boundary.
    TileType tileAt(int col, int row) const;

    // Returns the number of tile columns in the level (horizontal extent).
    // Renderer::draw() uses this to know how many columns to iterate when
    // drawing the map. Player::update() uses it for boundary clamping.
    int width() const;

    // Returns the number of tile rows in the level (vertical extent).
    // Same consumers as width().
    int height() const;

    // Writes the column and row of the Spawn tile into the provided references.
    // Called once in main() to set the Player's initial (and respawn) position.
    // If no Spawn tile exists in the layout, col and row are set to 0.
    void spawnPosition(int& col, int& row) const;

private:
    std::vector<std::string> _rows; // parsed layout, one string per row
    int _width;                     // cached column count
    int _height;                    // cached row count
};
