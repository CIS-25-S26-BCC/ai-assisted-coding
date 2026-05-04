#include "renderer.h"

#include <cmath>
#include <iostream>
#include <sstream>

namespace {

char tileToChar(TileType tile) {
	switch (tile) {
		case TileType::Solid:
			return '#';
		case TileType::Hazard:
			return 'X';
		case TileType::Goal:
			return 'G';
		case TileType::Spawn:
			return 'S';
		case TileType::Empty:
		default:
			return '.';
	}
}

const char* stateToText(GameState state) {
	switch (state) {
		case GameState::Running:
			return "Running";
		case GameState::Won:
			return "Won";
		case GameState::Dead:
			return "Dead";
		case GameState::Quit:
			return "Quit";
		default:
			return "Unknown";
	}
}

} // namespace

void Renderer::draw(const Map& map, const Player& player, GameState state) {
	std::ostringstream out;
	out << "\033[H";

	const int pCol = static_cast<int>(std::floor(player.x));
	const int pRow = static_cast<int>(std::floor(player.y));

	for (int row = 0; row < map.height(); ++row) {
		for (int col = 0; col < map.width(); ++col) {
			char ch = tileToChar(map.tileAt(col, row));
			if (row == pRow && col == pCol) {
				ch = '@';
			}
			out << ch;
		}
		out << '\n';
	}

	out << "[A/D] Move  [W/Space] Jump  [Q] Quit  | Status: " << stateToText(state) << '\n';
	std::cout << out.str() << std::flush;
}
