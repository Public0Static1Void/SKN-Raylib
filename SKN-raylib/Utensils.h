#pragma once
#include <raylib.h>

class ObjectTransform {
public:
	ObjectTransform() = default;
	ObjectTransform(Vector2 position, Vector2 size)
		: position(position), size(size) {}
	ObjectTransform(Vector2 position, Vector2 size, int rotation)
		: position(position), size(size), rotation(rotation) {}

	~ObjectTransform() = default;

	static Vector2 DirFromRotation(float rotation) {
		float rad = rotation * DEG2RAD;
		return { roundf(sinf(rad)), roundf(-cosf(rad)) };
	}
	static float RotationFromDir(Vector2 dir) {
		float deg = atan2f(dir.x, -dir.y) * RAD2DEG;
		if (deg < 0) deg += 360;
		return roundf(deg);
	}

	Vector2 position{ 0, 0 };
	Vector2 size{ 1, 1 };
	int rotation = 90;
};