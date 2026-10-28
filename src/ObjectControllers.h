#pragma once
#include "Object.h"

class PlayerController : public Object {
public:
	// This class will always be the player
	PlayerController(Grid* grid, Color drawColor, int max_life)
		: Object(true, EntityType::PLAYER, grid, drawColor, max_life)
	{
		sprite = Sprite::PLAYER;
	}

	Action Decide(const Vector2& player_pos, ObjectHandler& objectHandler) override {
		Action ac;
		// Stop the logic if the object is dead
		if (!IsAlive() || IsAnimating()) return ac;

		Vector2 dir{ 0, 0 };
		if (IsKeyPressed(KEY_A)) dir.x = -1;
		if (IsKeyPressed(KEY_D)) dir.x = 1;
		if (IsKeyPressed(KEY_W)) dir.y = -1;
		if (IsKeyPressed(KEY_S)) dir.y = 1;

		ac = SetUpActionType(dir);

		current_action = ac;
		return ac;
	}

	void OnCollision(Object& other) override {
		Vector2 dir = ObjectTransform::DirFromRotation(objectTransform.rotation);
		AnimateTo(objectTransform.position + dir * (grid->tile_size / 4), 0.75f, Easing::OutBack, true);
	}
	void OnWallHit() override {
		OnCollision(*this);
	}
};

class EnemyController : public Object {
public:
	EnemyController(Grid* grid, Color drawColor, int max_life)
		: Object(false, EntityType::ENEMY, grid, drawColor, max_life)
	{
		sprite = Sprite::ENEMY;
	}

	Action Decide(const Vector2& player_pos, ObjectHandler& objectHandler) override {
		Action ac;
		// Stop the logic if the object is dead or animating
		if (!IsAlive() || IsAnimating()) return ac;

		Vector2 dir{ 0, 0 };

		PathResult result = FindPathBFS(
			{ objectTransform.position.x + grid->tile_size / 2, objectTransform.position.y + grid->tile_size / 2 },
			{ player_pos.x + grid->tile_size / 2, player_pos.y + grid->tile_size / 2 },
			grid, objectHandler.Objects()
		);

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
	TargetController(Grid* grid, Color drawColor, int max_life)
		: Object(false, EntityType::TARGET, grid, drawColor, max_life) {
		sprite = Sprite::TARGET;
	}

	Action Decide(const Vector2& player_pos, ObjectHandler& objectHandler) override {
		return Action();
	}

	void OnHit(Object& other) override {
		if (other.type == EntityType::BULLET)
			if (on_hit) on_hit();
		AnimateSize(objectTransform.size * 1.4f, 0.35f, Easing::OutBack, true);
	}
};

class DoorController : public Object {
public:
	DoorController(Grid* grid, Color drawColor, int max_life)
		: Object(false, EntityType::DOOR, grid, drawColor, max_life) {
		sprite = Sprite::DOOR;
	}

	Action Decide(const Vector2& player_pos, ObjectHandler& objectHandler) override {
		return Action();
	}

	void Interact(int override_value = -1) override {
		if (override_value > -1) {
			bool new_v = override_value == 0 ? true : false;
			SetCanCollide(new_v);
			SetCanRender(new_v);
		}
		else {
			SetCanCollide(!GetCanCollide());
			SetCanRender(!GetCanRender());
		}
	}

	void OnHit(Object& other) override {
		if (!GetCanCollide() && other.type == EntityType::PLAYER)
			if (on_hit) on_hit();
	}
};