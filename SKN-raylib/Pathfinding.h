#pragma once
#include <raylib.h>

class Grid;

struct PathResult {
	Vector2 dir{0, 0};
	int steps_left = 0;
};

/// <summary>
/// Returns the direction of the first step to reach a target
/// </summary>
PathResult FindPathBFS(Vector2 start, Vector2 end, Grid* grid);