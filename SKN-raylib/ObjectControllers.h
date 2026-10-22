#pragma once
#include "Object.h"

class PlayerController : public Object {
public:
	// This class will always be the player
	PlayerController(Grid* grid, Color drawColor, int max_life) : Object(true, EntityType::PLAYER, grid, drawColor, max_life)
	{
		sprite = Sprite::PLAYER;
	}

	Action Decide(const Vector2& player_pos, ObjectHandler& objectHandler) override {
		Action ac;
		// Stop the logic if the object is dead
		if (!IsAlive() || is_moving) return ac;

		Vector2 dir{ 0, 0 };
		if (IsKeyPressed(KEY_A)) dir.x = -1;
		if (IsKeyPressed(KEY_D)) dir.x = 1;
		if (IsKeyPressed(KEY_W)) dir.y = -1;
		if (IsKeyPressed(KEY_S)) dir.y = 1;

		ac = SetUpActionType(dir);

		current_action = ac;
		return ac;
	}

	void Die() override {
		on_die();
	}

	std::function<void()> on_die;
};

class EnemyController : public Object {
public:
	EnemyController(Grid* grid, Color drawColor, int max_life) : Object(false, EntityType::ENEMY, grid, drawColor, max_life)
	{
		sprite = Sprite::ENEMY;
	}

	Action Decide(const Vector2& player_pos, ObjectHandler& objectHandler) override {
		Action ac;
		// Stop the logic if the object is dead
		if (!IsAlive()) return ac;

		Vector2 dir{ 0, 0 };

		PathResult result = FindPathBFS(
			{ objectTransform.position.x + grid->tile_size / 2, objectTransform.position.y + grid->tile_size / 2 },
			{ player_pos.x + grid->tile_size / 2, player_pos.y + grid->tile_size / 2 },
			grid, objectHandler.Objects()
		);

		// Check if the player is outside the attack range
		if (result.steps_left >= 0)
			dir = result.dir;

		ac = SetUpActionType(dir);

		current_action = ac;
		return ac;
	}

	void OnCollision(Object& other) override {
		if (other.type == EntityType::PLAYER) {
			other.Damage(1);
		}
	}
};

class TargetController : public Object {
public:
	TargetController(Grid* grid, Color drawColor, int max_life) : Object(false, EntityType::TARGET, grid, drawColor, max_life) {
		sprite = Sprite::TARGET;
	}

	Action Decide(const Vector2& player_pos, ObjectHandler& objectHandler) override {
		return Action();
	}
};