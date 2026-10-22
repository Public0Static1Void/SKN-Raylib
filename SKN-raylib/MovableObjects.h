#pragma once
#include "Object.h"

class BulletController : public Object {
public:
	BulletController(Grid* grid, Color drawColor, int max_life, Vector2 move_direction)
		: Object(false, EntityType::BULLET, grid, drawColor, max_life), move_direction(move_direction)
	{
		sprite = Sprite::BULLET;
	}

	Action Decide(const Vector2& player_pos, ObjectHandler& objectHandler) override {
		Action ac = SetUpActionType(move_direction);
		current_action = ac;
		return ac;
	}

	void OnCollision(Object& other) override {
		if (other.type == EntityType::ENEMY || other.type == EntityType::PLAYER)
			other.Damage(1);
		OnWallHit();
	}
	void OnWallHit() override {
		// Autodestruct
		AnimateTo(objectTransform.position + move_direction * (grid->tile_size / 2), 0.15f, Easing::Linear, false,
			[this]() { 
				AnimateSize({ 0, 0 }, 0.25f, Easing::OutQuad, false, [this]() {
					SetLife(0);
					});
			});
	}

private:
	Vector2 move_direction{ 0, 0 };
};
class CannonController : public Object {
public:
	CannonController(Grid* grid, Color drawColor, int max_life) : Object(false, EntityType::CANNON, grid, drawColor, max_life)
	{
		sprite = Sprite::CANNON;
	}

	Action Decide(const Vector2& player_pos, ObjectHandler& objectHandler) override {
		Action ac;
		if (!is_moving)
		{
			current_cooldown--;
			if (current_cooldown <= 0) {
				// Spawn bullet
				Vector2 move_dir = ObjectTransform::DirFromRotation(objectTransform.rotation);
				auto& bullet = objectHandler.Spawn<BulletController>(grid, draw_color, 1, move_dir);
				bullet.SetPosition(objectTransform.position + move_dir * (grid->tile_size / 2));
				bullet.AnimateTo(objectTransform.position + move_dir * grid->tile_size, 0.5f, Easing::OutQuad);
				bullet.AnimateSize({ (float)grid->tile_size, (float)grid->tile_size }, 0.25f, Easing::OutBack);
				current_cooldown = turn_cooldown;
			}
		}
		current_action = ac;
		return ac;
	}

	void SetTurnCooldown(int new_cooldown) {
		turn_cooldown = new_cooldown;
		current_cooldown = new_cooldown;
	}

private:
	int turn_cooldown = 3, current_cooldown = 3;
	bool can_shoot = false;
};
class CrateController : public Object {
public:
	CrateController(Grid* grid, Color drawColor, int max_life) : Object(false, EntityType::CRATE, grid, drawColor, max_life)
	{
		sprite = Sprite::CRATE;
	}

	Action Decide(const Vector2& player_pos, ObjectHandler& objectHandler) override {
		return Action();
	}
};