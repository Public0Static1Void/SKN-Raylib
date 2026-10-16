#pragma once
#include "Utensils.h"
#include "Grid.h"
#include "Pathfinding.h"
#include "ObjectHandler.h"
#include "Assets.h"
#include <raymath.h>
#include <iostream>
#include <string>
#include <optional>

enum class Facing { UP, DOWN, RIGHT, LEFT };

struct Action {
	enum class Type { NONE, MOVE, ROTATE } type = Type::NONE;
	Vector2 dir{0, 0};
	int degrees = 0; // For rotations
};
class Object;
struct CollisionInfo {
	bool collided = false;
	Vector2 collision_point{ 0, 0 };
	int collided_object_id = -1;
	Object* collided_object = nullptr;
};

class LifeSystem {
public:
	LifeSystem(int max_life) : max_life(max_life), life(max_life) {}
	virtual ~LifeSystem() = default;

	void Damage(int amount) {
		life -= amount;
		if (life <= 0) Die();
	}
	void SetLife(int amount) {
		if (amount > max_life) amount = max_life;
		life = amount;
		if (life <= 0) Die();
	}
	virtual void Die() {}

	bool IsAlive() const { return life > 0; }

private:
	int max_life = 3, life = 3;
};

class Object : public LifeSystem {
public:
	Object(bool isPlayer, EntityType type, Grid* grid, Color drawColor, int max_life) : isPlayer(isPlayer), type(type), grid(grid), draw_color(drawColor), LifeSystem(max_life) {}
	virtual ~Object() = default;

	virtual Action Decide(const Vector2& player_pos, ObjectHandler& objectHandler) = 0;
	virtual void Act(std::vector<std::unique_ptr<Object>>& objects) {
		if (current_action.type == Action::Type::NONE) return;

		switch (current_action.type) {
		case Action::Type::MOVE: {
			Vector2 new_pos = objectTransform.position + current_action.dir * grid->tile_size;
			CollisionInfo hit = ObjectCollision(new_pos, objects);
			if (hit.collided && hit.collided_object)
				OnCollision(*hit.collided_object);
			else if (grid->IsWalkable(new_pos))
				objectTransform.position = new_pos;
			else
				OnWallHit();

			// Rotate the object to face the movement direction
			objectTransform.rotation = ObjectTransform::RotationFromDir(current_action.dir);
			break;}
		case Action::Type::ROTATE:{
			objectTransform.rotation += current_action.degrees;
			// Make the rotation loop on 360
			if (objectTransform.rotation < 0) objectTransform.rotation += 360;
			else if (objectTransform.rotation >= 360) objectTransform.rotation -= 360;
			break;}
		}
	}

	void SetPosition(Vector2 pos) {
		objectTransform.position = pos;
	}
	void SetPosition(int x, int y) {
		objectTransform.position.x = x;
		objectTransform.position.y = y;
	}
	void SetSize(int size) {
		objectTransform.size.x = size;
		objectTransform.size.y = size;
	}
	void SetSize(int x, int y) {
		objectTransform.size.x = x;
		objectTransform.size.y = y;
	}
	void SetSize(Vector2 size) {
		objectTransform.size.x = size.x;
		objectTransform.size.y = size.y;
	}
	void SetId(int new_id) { id = new_id; }
    int GetId() const { return id; }

	void DrawObject(const Assets& assets) {
		if (!IsAlive()) return;

		Vector2 pos = objectTransform.position + grid->GetGridOffset();
		Vector2& size = objectTransform.size;

		// Center the rectangle on the tile
		float size_diff = grid->tile_size - (size.x + size.y) / 2.0f;
		pos += size / (grid->tile_size / size_diff);

		Rectangle dest = {
			pos.x + (grid->tile_size - size.x) / 2.0f,
			pos.y + (grid->tile_size - size.y) / 2.0f,
			size.x, size.y
		};
		
		//DrawRectangle(pos.x, pos.y, size.x, size.y, draw_color);
		DrawSprite(assets.tilemap, sprite, dest, objectTransform.rotation);
	}

	CollisionInfo ObjectCollision(Vector2 collision_pos, std::vector<std::unique_ptr<Object>>& objects) {
		for (const auto& ob : objects) {
			if (!ob->IsAlive()) continue;			// Dead object, skip
			if (ob->GetId() == GetId()) continue;	// Skip itself

			Vector2 diff = collision_pos - ob->objectTransform.position;
			int dist = (fabsf(diff.x) + fabsf(diff.y)) / grid->tile_size; // Get the distance between the collision point and the other object

			if (dist <= 0)
				return { true, ob->objectTransform.position, ob->GetId(), ob.get() };
		}
		return { false, {0, 0}, -1, nullptr };
	}

	virtual void OnCollision(Object& other) {}
	virtual void OnWallHit() {}

	ObjectTransform objectTransform;
	bool isPlayer = false;
	EntityType type;

protected:
	Action current_action;
	Grid* grid;
	Color draw_color;

	Action SetUpActionType(Vector2 dir) {
		Action ac;
		ac.dir = dir;
		ac.type = Vector2Equals(dir, Vector2{ 0, 0 })
			? Action::Type::NONE : Action::Type::MOVE;
		return ac;
	}

	Sprite sprite = Sprite::LAST_NO_USE;

private:
	int id = -1;
};

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
		if (!IsAlive()) return ac;

		Vector2 dir{0, 0};
		if (IsKeyPressed(KEY_A)) dir.x = -1;
		if (IsKeyPressed(KEY_D)) dir.x = 1;
		if (IsKeyPressed(KEY_W)) dir.y = -1;
		if (IsKeyPressed(KEY_S)) dir.y = 1;

		ac = SetUpActionType(dir);

		current_action = ac;
		return ac;
	}
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

		Vector2 dir{0, 0};

		PathResult result = FindPathBFS(
			{ objectTransform.position.x + grid->tile_size / 2, objectTransform.position.y + grid->tile_size / 2 },
			{ player_pos.x + grid->tile_size / 2, player_pos.y + grid->tile_size / 2 },
			grid
		);

		// Check if the player is in attack range
		if (result.steps_left >= 0)
			dir = result.dir;
		else {

		}
		
		ac = SetUpActionType(dir);

		current_action = ac;
		return ac;
	}

	void OnCollision(Object& other) override {
		std::cout << "Enemy attempting damage" << std::endl;
		if (other.type == EntityType::PLAYER) {
			other.Damage(1);
		}
	}
};


class BulletController : public Object {
public:
	BulletController(Grid* grid, Color drawColor, int max_life, Vector2 move_direction)
		: Object(false, EntityType::NONE, grid, drawColor, max_life), move_direction(move_direction)
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
		SetLife(0);
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

		current_cooldown--;
		if (current_cooldown <= 0) {
			// Spawn bullet
			Vector2 move_dir = ObjectTransform::DirFromRotation(objectTransform.rotation);
			auto& bullet = objectHandler.Spawn<BulletController>(grid, draw_color, 1, move_dir);
			bullet.SetPosition(objectTransform.position + move_dir * grid->tile_size);
			bullet.SetSize(grid->tile_size);
			current_cooldown = turn_cooldown;
		}

		return ac;
	}

	void Act(std::vector<std::unique_ptr<Object>>& objects) override {

	}

	void SetTurnCooldown(int new_cooldown) {
		turn_cooldown = new_cooldown;
		current_cooldown = new_cooldown;
	}

private:
	int turn_cooldown = 3, current_cooldown = 3;
	bool can_shoot = false;
};