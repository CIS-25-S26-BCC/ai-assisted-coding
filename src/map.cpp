#include "map.h"

#include <algorithm>

namespace {

char sanitizeTileChar(char c) {
	switch (c) {
		case '#':
		case '.':
		case 'X':
		case 'G':
		case 'S':
			return c;
		default:
			return '.';
	}
}

} // namespace

Map::Map(const std::string& layout) : _width(0), _height(0) {
	std::string current;
	for (char c : layout) {
		if (c == '\r') {
			continue;
		}
		if (c == '\n') {
			if (!current.empty()) {
				_rows.push_back(current);
				current.clear();
			}
			continue;
		}
		current.push_back(sanitizeTileChar(c));
	}
	if (!current.empty()) {
		_rows.push_back(current);
	}

	if (_rows.empty()) {
		_rows = {
			"############",
			"#S.......G.#",
			"#....X.....#",
			"#..........#",
			"############",
		};
	}

	for (const std::string& row : _rows) {
		_width = std::max(_width, static_cast<int>(row.size()));
	}
	_height = static_cast<int>(_rows.size());

	for (std::string& row : _rows) {
		if (static_cast<int>(row.size()) < _width) {
			row.append(static_cast<std::size_t>(_width - static_cast<int>(row.size())), '.');
		}
	}
}

TileType Map::tileAt(int col, int row) const {
	if (col < 0 || row < 0 || col >= _width || row >= _height) {
		return TileType::Solid;
	}

	const char c = _rows[static_cast<std::size_t>(row)][static_cast<std::size_t>(col)];
	switch (c) {
		case '#':
			return TileType::Solid;
		case 'X':
			return TileType::Hazard;
		case 'G':
			return TileType::Goal;
		case 'S':
			return TileType::Spawn;
		case '.':
		default:
			return TileType::Empty;
	}
}

int Map::width() const { return _width; }
int Map::height() const { return _height; }

void Map::spawnPosition(int& col, int& row) const {
	for (int r = 0; r < _height; ++r) {
		for (int c = 0; c < _width; ++c) {
			if (_rows[static_cast<std::size_t>(r)][static_cast<std::size_t>(c)] == 'S') {
				col = c;
				row = r;
				return;
			}
		}
	}

	col = 0;
	row = 0;
}
