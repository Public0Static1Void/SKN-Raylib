#include "ObjectHandler.h"
#include <raymath.h>
#include <algorithm>
#include <iostream>
#include "Object.h"

ObjectHandler::ObjectHandler() = default;
ObjectHandler::~ObjectHandler() = default;

void ObjectHandler::EndTurn() {
	// Add all the pending objects
	for (auto& p : pending)
		objects.push_back(std::move(p));
	pending.clear();

	// Erase all the objects that are dead
	objects.erase(
		std::remove_if(objects.begin(), objects.end(),
			[](const std::unique_ptr<Object>& ob) { return !ob->GetLifeSystem()->IsAlive(); }),
		objects.end()
	);
}

void ObjectHandler::RemoveType(EntityType type) {
	pending.clear();

	objects.erase(std::remove_if(objects.begin(), objects.end(),
		[type](const std::unique_ptr<Object>& ob) { return ob->type == type; }),
		objects.end()
	);
}

void ObjectHandler::InteractWithObject(EntityType type) {
	for (auto& ob : objects) {
		if (ob->type == type) ob->Interact();
	}
}

CollisionInfo ObjectHandler::ObjectCollision(Vector2 collision_pos, Object& self, std::vector<std::unique_ptr<Object>>& objects, bool ignoreSelf, const std::vector<EntityType>& excludeTypes)
{
	auto isExcluded = [&](const Object* ob) {
		for (const EntityType& ex : excludeTypes) {
			if (ob->type == ex) return true;
		}
		return false;
		};

	for (const auto& ob : objects) {
		if (!ob->GetLifeSystem()->IsAlive()) continue;
		if (ignoreSelf && ob->GetId() == self.GetId()) continue; // Skip itself
		if (isExcluded(ob.get())) continue; // Skip excluded entity types

		Vector2 diff = collision_pos - ob->objectTransform.position;

		int dist = static_cast<int>((std::fabs(diff.x) + std::fabs(diff.y)) / self.GetGrid()->tile_size);
		if (dist <= 0) {
			//if (!ob->GetCanCollide()) ob->OnHit(*this);
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