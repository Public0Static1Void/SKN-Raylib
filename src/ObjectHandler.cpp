#include "ObjectHandler.h"
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
			[](const std::unique_ptr<Object>& ob) { return !ob->IsAlive(); }),
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