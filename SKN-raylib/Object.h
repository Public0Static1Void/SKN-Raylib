#pragma once
#include "Utensils.h"
#include <raymath.h>

enum class Facing { UP, DOWN, RIGHT, LEFT };
enum class EntityType { PLAYER, ENEMY, EDITABLE };

struct Action {
	enum class Type { NONE, MOVE, ROTATE } type = Type::NONE;
	Vector2 dir;
	int degrees = 0; // For rotations
};

class Object {
public:
	Object() = default;
	virtual ~Object() = default;

	virtual Action Decide() = 0;

	Transform transform;
};

class PlayerController : public Object {
public:
	Action Decide() override {
		Action ac;
		Vector2 dir;
		if (IsKeyPressedRepeat(KEY_A)) dir.x = 1;
		if (IsKeyPressedRepeat(KEY_D)) dir.x = -1;
		if (IsKeyPressedRepeat(KEY_W)) dir.y = -1;
		if (IsKeyPressedRepeat(KEY_S)) dir.y = 1;

		ac.dir = dir;
		ac.type = Vector2Equals(dir, Vector2{ 0, 0 }) 
			? Action::Type::NONE : Action::Type::MOVE;
		return ac;
	}
};