#pragma once
#include <raylib.h>

class Transform {
public:
	Transform() = default;
	Transform(Vector2 position, Vector2 size)
		: position(position), size(size) {}
	Transform(Vector2 position, Vector2 size, float rotation)
		: position(position), size(size), rotation(rotation) {}

	~Transform() = default;

	Vector2 position{ 0, 0 };
	Vector2 size{ 0, 0 };
	float rotation = 0;
};