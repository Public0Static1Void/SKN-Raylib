#pragma once
#include "Object.h"
#include "ObjectHandler.h"

class TurnManager {
public:
	enum class Phase { WAIT_INPUT, PLAYER_ACTING, WORLD_ACTING };

	void Update(ObjectHandler& objectHandler, Object& player, bool skip_turn) {
		auto& objects = objectHandler.Objects();

		switch (current_phase) {
		case Phase::WAIT_INPUT:
		{
			const Action ac = player.Decide(player.objectTransform.position, objectHandler);
			if (ac.type == Action::Type::NONE && !skip_turn) return;
			current_turn++;

			player.Act(objects);
			current_phase = Phase::PLAYER_ACTING;

			break;
		}
		case Phase::PLAYER_ACTING:
		{
			ActWorld(objectHandler, player);
			current_phase = Phase::WORLD_ACTING;
			break;
		}
		case Phase::WORLD_ACTING:
			if (player.IsAnimating()) return; // Wait for the player to finish animating
			current_phase = Phase::WAIT_INPUT;
			break;
		}
	}

	void SetGrid(Grid* g) { grid = g; }

private:
	void ActWorld(ObjectHandler& objectHandler, Object& player) {
		const Vector2 playerPos = grid->SnapToTile(player.objectTransform.position);
		auto& objects = objectHandler.Objects();
		for (auto& ob : objects) {
			if (!ob->IsAlive() || ob.get() == &player) continue;

			ob->Decide(playerPos, objectHandler);
			ob->Act(objects);
		}
	}

	static bool AnyAnimating(std::vector<std::unique_ptr<Object>>& objects) {
		for (auto& ob : objects) {
			if (ob->IsAlive() && ob->IsAnimating()) return true;
		}
		return false;
	}

	Phase current_phase = Phase::WAIT_INPUT;
	int current_turn = 0;

	Grid* grid;
};