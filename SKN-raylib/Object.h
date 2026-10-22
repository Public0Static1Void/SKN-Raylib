#pragma once
#include "Utensils.h"
#include "Grid.h"
#include "Pathfinding.h"
#include "ObjectHandler.h"
#include "Assets.h"
#include "Animator.h"
#include <raymath.h>
#include <iostream>
#include <string>
#include <optional>
#include <functional>
#include <algorithm>

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
	Object(bool isPlayer, EntityType type, Grid* grid, Color drawColor, int max_life) 
		: isPlayer(isPlayer), type(type), grid(grid), draw_color(drawColor), LifeSystem(max_life) 
	{}
	virtual ~Object() = default;

	virtual Action Decide(const Vector2& player_pos, ObjectHandler& objectHandler) = 0;
	virtual void Act(std::vector<std::unique_ptr<Object>>& objects) {
		if (anim.IsAnimating()) return;

		if (queued_action.type != Action::Type::NONE) {
			current_action = queued_action;
			queued_action.type = Action::Type::NONE;
		}
			
		if (current_action.type == Action::Type::NONE) return;

		switch (current_action.type) {
		case Action::Type::MOVE: {
			Vector2 new_pos = objectTransform.position + current_action.dir * grid->tile_size;
			CollisionInfo hit = ObjectCollision(new_pos, objects);
			if (hit.collided && hit.collided_object) {
				OnCollision(*hit.collided_object);
				hit.collided_object->OnHit(*this);
			}

			else if (grid->IsWalkable(new_pos))
				AnimateTo(new_pos, DEFAULT_MOVE_DURATION, Easing::OutQuad);
			else
				OnWallHit();

			// Rotate the object to face the movement direction
			AnimateRotation(ObjectTransform::RotationFromDir(current_action.dir), DEFAULT_MOVE_DURATION, Easing::OutQuad);

			break;}
		case Action::Type::ROTATE:{
			AnimateRotation(objectTransform.rotation + current_action.degrees, DEFAULT_MOVE_DURATION, Easing::OutQuad);
			break;}
		}

		// Avoid repetitive actions
		current_action.type = Action::Type::NONE;

		// Remove an action if it's an interactable object
		if (moves_left != -1) moves_left--;
	}

	void UpdateMovement(float deltaTime) {
		anim.Update(deltaTime, objectTransform);
	}

	void SetPosition(Vector2 pos) {
		if (is_moving) return;
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

		Rectangle dest = {
			pos.x + (grid->tile_size - size.x) / 2.0f,
			pos.y + (grid->tile_size - size.y) / 2.0f,
			size.x, size.y
		};
		
		//DrawRectangle(pos.x, pos.y, size.x, size.y, draw_color);
		DrawSprite(assets.tilemap, sprite, dest, objectTransform.rotation);
	}

	CollisionInfo ObjectCollision(Vector2 collision_pos, std::vector<std::unique_ptr<Object>>& objects, bool ignoreSelf = true, std::vector<EntityType> excludeTypes = {}) {
		auto isExcluded = [&](const Object* ob) {
			for (const EntityType& ex : excludeTypes) {
				if (ob->type == ex) return true;
			}
			return false;
			};

		for (const auto& ob : objects) {
			if (!ob->IsAlive()) continue; // Dead object, skip
			if (ignoreSelf && ob->GetId() == GetId()) continue; // Skip itself
			if (isExcluded(ob.get())) continue; // Skip excluded entity types

			Vector2 diff = collision_pos - ob->objectTransform.position;

			int dist = static_cast<int>((std::fabs(diff.x) + std::fabs(diff.y)) / grid->tile_size);
			if (dist <= 0) {
				return {
					true,
					ob->objectTransform.position,
					ob->GetId(),
					ob.get()
				};
			}
		}

		return { false, { 0, 0 }, -1, nullptr };
	}

	virtual void OnCollision(Object& other) {}
	virtual void OnHit(Object& other) {
		if (other.type == EntityType::BULLET) {
			AnimateSize(objectTransform.size * 1.4f, 0.35f, Easing::OutBack, true,
				[this] { if (on_hit) on_hit(); }
			);
		}
	}
	virtual void OnWallHit() {}

	void QueueAction(Action& ac) { queued_action = ac; }
	void ExecuteAction(Action& ac, std::vector<std::unique_ptr<Object>>& objects) {
		if (moves_left == -1 || moves_left > 0) {
			if (ac.type == Action::Type::MOVE && Vector2Equals(ac.dir, { 0, 0 })) {
				ac.dir = ObjectTransform::DirFromRotation(objectTransform.rotation);
			}
			queued_action = ac;
			Act(objects);
		}
	}

	void SetInteractable(bool in) { is_interactable = in; moves_left = AVALIABLE_MOVES; }
	bool IsInteractabe() { return is_interactable; }

	void SetMoves(int moves) { moves_left = moves; }
	const int GetMoves() const { return moves_left; }

	void AnimateTo(Vector2 target_pos, float duration, EaseFn fn, bool returns = false, std::function<void()> end_event = nullptr) {
		if (!anim.IsMoving())
			anim.MoveTo(objectTransform.position, target_pos, duration, fn, returns, end_event);
	}
	void AnimateSize(Vector2 target_size, float duration, EaseFn fn, bool returns = false, std::function<void()> end_event = nullptr) {
		if (!anim.IsScaling())
			anim.ScaleTo(objectTransform.size, target_size, duration, fn, returns, end_event);
	}
	void AnimateRotation(float target_rotation, float duration, EaseFn fn, bool returns = false, std::function<void()> end_event = nullptr) {
		if (!anim.IsRotating())
			anim.RotateTo(objectTransform.rotation, target_rotation, duration, fn, returns, end_event);
	}

	ObjectTransform objectTransform;
	bool isPlayer = false;
	EntityType type;

	static constexpr int AVALIABLE_MOVES = 3;

	std::function<void()> on_hit;
protected:
	Action current_action, queued_action;
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
	
	bool is_moving = false;

private:
	int id = -1;
	bool is_interactable = false;
	int moves_left = -1;

	const float DEFAULT_MOVE_DURATION = 0.5f;
	Animator anim;
};

