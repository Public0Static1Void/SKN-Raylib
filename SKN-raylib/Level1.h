#pragma once
#include "Scene.h"
#include "Level2.h"
#include "Object.h"
#include "Grid.h"
#include "ObjectHandler.h"
#include "Assets.h"
#include "CameraUtils.h"
#include <raylib.h>
#include <vector>

class Level1 : public Scene
{
public:

	void Start() override {
		// Load the tileset
		assets.Load("Sprites/tilemap.png");
		// Load the map
		grid.SetSize(64);
		grid.ScanLevel("Level1.txt");
		std::vector<Grid::ObjectPosition> positions = grid.GetStartPositions();

		auto& player = objectHandler.Spawn<PlayerController>(&grid, GREEN, 3);
		player.SetSize(grid.tile_size);
		player_ptr = &player; // Store the player ptr

		auto& enemy = objectHandler.Spawn<EnemyController>(&grid, RED, 3);
		enemy.SetSize(grid.tile_size);

		auto& cannon = objectHandler.Spawn<CannonController>(&grid, YELLOW, 1);
		cannon.SetSize(grid.tile_size);

		objectHandler.EndTurn();

		auto& objects = objectHandler.Objects();
		for (int i = 0; i < objects.size(); i++) {
			for (int j = 0; j < positions.size(); j++) {
				if (objects[i]->type == positions[j].type) {
					objects[i]->SetPosition(positions[j].position * grid.tile_size);
					// Deassign the position type so other objects can assign it to themselves
					positions[j].type = EntityType::NONE;
				}
			}
		}		
	}

	void Update(float deltaTime) override {
		start = IsKeyPressed(KEY_SPACE);

		auto& objects = objectHandler.Objects();
		for (auto& ob : objects) {
			Action ac = ob->Decide(player_ptr->objectTransform.position, objectHandler);
			// If the object is the player and he isn't moving then end the logic
			if (ob->isPlayer && ac.type == Action::Type::NONE) return;

			ob->Act(objects);
		}
		objectHandler.EndTurn();
	}

	void Render() override {
		Camera2D cam2D = MakeFitCamera(grid.GetBounds());

		BeginMode2D(cam2D);
		grid.DrawGrid(assets);
		for (auto& ob : objectHandler.Objects()) {
			ob->DrawObject(assets);
		}
		EndMode2D();

		DrawText("Level 1 Scene", 0, 0, 10, GREEN);
	}

	std::unique_ptr<Scene> NextScene() override {
		return start ? std::make_unique<Level2>() : nullptr;
	}
private:
	bool start = false;
	std::vector<std::unique_ptr<Object>> objects;
	
	Grid grid;
	ObjectHandler objectHandler;

	PlayerController* player_ptr;

	Assets assets;
};