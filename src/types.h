#pragma once

// TileType represents every meaningful cell in the level grid.
// Map::tileAt() returns one of these values, and both Player and Renderer
// consume it — Player to decide collision/death/win logic, Renderer to
// choose which character to draw for each cell.
enum class TileType {
    Empty,   // passable air — player falls through freely
    Solid,   // impassable wall or floor — player collides with it
    Hazard,  // kills the player on contact, triggering respawn
    Goal,    // reaching this tile transitions state to GameState::Won
    Spawn,   // marks the player's starting position (read once at startup)
};

// GameState is the top-level mode of the game, owned by main().
// main() checks it after every update to decide whether to keep looping,
// show a win message, show a death/respawn message, or exit cleanly.
enum class GameState {
    Running, // normal play — input, update, and render every frame
    Won,     // player reached the Goal tile — show win screen, then exit or restart
    Dead,    // player hit a Hazard — brief pause, then call Player::respawn()
    Quit,    // player pressed Q — main() exits the loop and restores the terminal
};
