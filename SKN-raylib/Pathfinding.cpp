#include "Pathfinding.h"
#include <raymath.h>
#include <queue>
#include <string>
#include "Grid.h"
#include "Utensils.h"
#include "Object.h"

/// <summary>
/// Returns the direction of the first step to reach a target
/// </summary>
PathResult FindPathBFS(Vector2 start, Vector2 end, Grid* grid, std::vector<std::unique_ptr<Object>>& objects) {
	// Get the path to the target -- BFS algorithm
	Vector2 dir{ 0, 0 };
	int steps_left = 0;

	struct Cell {
		Vector2 position;
		Vector2 parent;
	};

	std::vector<Cell> visited_cells;
	std::queue<Cell> queue;
	std::vector<Vector2> path;
	queue.push({ start, start });
	visited_cells.push_back({ start, start });

	grid->searched_tiles.clear();

	auto isVisited = [&](Vector2 p) {
		bool walkable = grid->IsWalkable(p);
		if (walkable)
			for (const auto& ob : objects) {
				Vector2 coll_pos = { p.x - grid->tile_size / 2, p.y - grid->tile_size / 2 };
				CollisionInfo hit = ob->ObjectCollision(coll_pos, objects, false, { EntityType::PLAYER });
				if (hit.collided) {
					walkable = false;
					break;
				}
			}
		for (const Cell& v : visited_cells)
			if ((Vector2Equals(v.position, p) && walkable) || !walkable) return true;
		return false;
		};
	auto parent_of = [&](Vector2 p) {
		for (const Cell& v : visited_cells)
			if (Vector2Equals(v.position, p)) return v.parent;
		return p;
		};

	Vector2 dirs[] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
	bool path_found = false;
	while (!path_found && queue.size() > 0) {
		Cell curr = queue.front();
		for (int i = 0; i < 4; i++) {
			Vector2 next_pos = curr.position + dirs[i] * grid->tile_size;
			// Check if the cell is visited
			if (!isVisited(next_pos)) {
				visited_cells.push_back({ next_pos, curr.position });
				grid->searched_tiles.push_back(next_pos);
				queue.push({ next_pos, curr.position });
				// The algorithm has found the end
				if (Vector2Equals(end, next_pos)) {
					path_found = true;
					break;
				}
			}
		}
		queue.pop();
	}

	if (path_found) {
		// Build the path from the last point
		Vector2 step = end;
		std::vector<Vector2> path;
		while (!Vector2Equals(start, parent_of(step))) {
			step = parent_of(step);
			path.push_back(step);
			grid->IsWalkable(step, true);
		}
		// Get the direction from the first point to the second
		Vector2 diff = step - start;
		dir = { diff.x / grid->tile_size, diff.y / grid->tile_size };

		// Get the number of steps on the path
		steps_left = path.size();

		grid->path.clear();
		for (const auto& c : path) {
			grid->path.push_back(c);
		}
	}
	return { dir, steps_left };
}