#pragma once
#include <raylib.h>
#include <algorithm>
#include <cmath>

inline Camera2D MakeFitCamera(Rectangle world, float margin = 0.9f) {
	Camera2D cam{};
	const float sw = (float)GetScreenWidth();
	const float sh = (float)GetScreenHeight();

	float zoom = std::min(sw / world.width, sh / world.height) * margin;
	if (zoom >= 1.0f) zoom = std::floor(zoom);

	cam.zoom = zoom;
	cam.target = { world.x + world.width / 2.0f, world.y + world.height / 2.0f };
	cam.offset = { sw / 2.0f, sh / 2.0f };
	return cam;
}