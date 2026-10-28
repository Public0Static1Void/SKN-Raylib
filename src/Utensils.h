#pragma once
#include <raylib.h>
#include <raymath.h>

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
		deg = fmodf(fmodf(deg, 360.0f) + 360.0f, 360.0f);
		return roundf(deg);
	}
	static inline float ShortestAngleDelta(float from, float to) {
		float d = fmodf(to - from, 360.0f);
		if (d > 180.0f) d -= 360.0f;
		else if (d < -180.0f) d += 360.0f;
		return d;
	}

	Vector2 position{ 0, 0 };
	Vector2 size{ 1, 1 };
	int rotation = 90;
};