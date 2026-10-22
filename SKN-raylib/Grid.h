#pragma once
#include <iostream>
#include <vector>
#include <raylib.h>
#include <string>

class Assets;

enum class EntityType { PLAYER, ENEMY, CANNON, CRATE, BULLET, TARGET, NONE };

class Grid
{
public:
	bool ScanLevel(const char* fileName);

	void DrawGrid(const Assets& assets);

	void SetSize(int tileSize) {
		tile_size = tileSize;
	}

	struct ObjectPosition {
		Vector2 position{0, 0};
		EntityType type = EntityType::NONE;
	};

	const std::vector<ObjectPosition>& GetStartPositions() const {
		return objects_start_positions;
	}

	const Vector2 GetEntityPosition(EntityType type) const {
		for (int i = 0; i < objects_start_positions.size(); i++) {
			if (objects_start_positions[i].type == type) {
				return objects_start_positions[i].position;
			}
		}
		return { 0, 0 };
	}

	Vector2 GetGridOffset() { return start_draw_position; }

	bool IsWalkable(Vector2 position, bool log = false) {
		Vector2 stored_pos = position;
		position = { position.x / tile_size, position.y / tile_size };
		if (position.y >= tiles.size() || position.y < 0 || position.x >= tiles[position.y].size() || position.x < 0) return false;
		if (log)
			std::cout << "Position: " << std::to_string(stored_pos.x) << ", " << std::to_string(stored_pos.y) << " | Tile: " << tiles[position.y][position.x] << std::endl;
		return tiles[position.y][position.x] != '1';
	}

	Rectangle GetBounds() const {
		if (tiles.empty()) return { 0, 0, 1, 1 };
		Rectangle bounds = {
			(float)start_draw_position.x, (float)start_draw_position.y,
			(float)(tiles[0].size() * tile_size), (float)(tiles.size() * tile_size)
		};
		return bounds;
	}

	int tile_size = 100;
	std::vector<Vector2> path;
	std::vector<Vector2> searched_tiles;
private:
	Vector2 start_draw_position{ 0, 0 };

	std::vector<std::string> tiles;
	std::vector<ObjectPosition> objects_start_positions;
};