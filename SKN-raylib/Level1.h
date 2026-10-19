#pragma once
#include "Scene.h"

#include "Level2.h"
#include "WinScene.h"
#include "GameOverScene.h"
#include "Object.h"
#include "MovableObjects.h"
#include "InteractiveOverlay.h"
#include "Grid.h"
#include "ObjectHandler.h"
#include "Assets.h"
#include "CameraUtils.h"

#include <raylib.h>
#include <vector>
#include <functional>
#include <memory>

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

		for (int i = 0; i < positions.size(); i++) {
			switch (positions[i].type) {
			case EntityType::PLAYER: 
			{
				auto& player = objectHandler.Spawn<PlayerController>(&grid, GREEN, 3);
				player_ptr = &player; // Store the player ptr
				player.on_die = [this]() { SetNextScene<GameOverScene>(); changeScene = true;  };
				break;
			}	
			case EntityType::ENEMY:
			{
				auto& enemy = objectHandler.Spawn<EnemyController>(&grid, RED, 3);
				break;
			}
			case EntityType::CANNON:
			{
				auto& cannon = objectHandler.Spawn<CannonController>(&grid, YELLOW, 1);
				cannon.SetInteractable(true);
				break;
			}
			case EntityType::CRATE:
			{
				auto& crate = objectHandler.Spawn<CrateController>(&grid, BROWN, 1);
				crate.SetInteractable(true);
				break;
			}
			case EntityType::TARGET:
			{
				auto& target = objectHandler.Spawn<TargetController>(&grid, RED, 1);
				target.on_hit = [this]() { SetNextScene<WinScene>(); changeScene = true; };
				break;
			}
			}
		}

		objectHandler.EndTurn();

		auto& objects = objectHandler.Objects();

		// Put the player object at the start of the vector
		auto it = std::find_if(objects.begin(), objects.end(),
			[this](const std::unique_ptr<Object>& ob) {
				return ob.get() == player_ptr;
			}
		);
		if (it != objects.begin()) {
			std::rotate(objects.begin(), it, it + 1);
		}

		for (int i = 0; i < objects.size(); i++) {
			objects[i]->SetSize(grid.tile_size);
			for (int j = 0; j < positions.size(); j++) {
				if (objects[i]->type == positions[j].type) {
					objects[i]->SetPosition(positions[j].position * grid.tile_size);
					// Deassign the position type so other objects can assign it to themselves
					positions[j].type = EntityType::NONE;
					break;
				}
			}
		}

		SetNextScene<Level2>();
	}

	void Update(float deltaTime) override {
		changeScene = IsKeyPressed(KEY_SPACE);

		auto& objects = objectHandler.Objects();

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);

			if (!overlay.IsVisible()) {
				interacted_object = FindInteractableAtPos(mouse_pos);
				if (interacted_object != nullptr) {
					Vector2 pos = interacted_object->objectTransform.position;
					overlay.SetAnchor(Vector2{ pos.x, pos.y + grid.tile_size });
					overlay.SetVisible(true);
				}
			}
			else {
				int idx = overlay.GetClickedOption(mouse_pos, grid);
				Action ac = overlay.GetIndexAction(idx);
				interacted_object->ExecuteAction(ac, objects);
				interacted_object = nullptr;
				overlay.SetVisible(false);
			}
		}

		for (auto& ob : objects) {
			Action ac = ob->Decide(player_ptr->objectTransform.position, objectHandler);
			// If the object is the player and he isn't moving then end the logic
			if (ob->isPlayer && ac.type == Action::Type::NONE) return;

			ob->Act(objects);
		}
		objectHandler.EndTurn();
	}

	void Render() override {
		camera = MakeFitCamera(grid.GetBounds());

		BeginMode2D(camera);
		grid.DrawGrid(assets);
		for (auto& ob : objectHandler.Objects()) {
			ob->DrawObject(assets);
		}

		overlay.DrawOverlay(assets, grid, interacted_object);
		EndMode2D();

		DrawText("Level 1 Scene", 0, 0, 10, GREEN);
	}

	std::unique_ptr<Scene> NextScene() override {
		if (!changeScene || !next_scene_factory) return nullptr;
		return next_scene_factory();
	}
private:
	bool changeScene = false;
	std::function<std::unique_ptr<Scene>()> next_scene_factory;
	template <typename T>
	void SetNextScene() {
		next_scene_factory = [] { return std::make_unique<T>(); };
	}

	std::vector<std::unique_ptr<Object>> objects;
	
	Grid grid;
	ObjectHandler objectHandler;

	PlayerController* player_ptr;

	Assets assets;

	Camera2D camera{};
	InteractiveOverlay overlay;
	Object* interacted_object;

	Object* FindById(int id) {
		for (auto& ob : objectHandler.Objects()) {
			if (id == ob->GetId()) return ob.get();
		}
		return nullptr;
	}
	Object* FindInteractableAtPos(const Vector2& pos) {
		const float tileSize = grid.tile_size;
		Vector2 local = pos - grid.GetGridOffset();
		int cx = (int)floorf(local.x / tileSize), cy = (int)floorf(local.y / tileSize);

		for (auto& ob : objectHandler.Objects()) {
			if (!ob->IsAlive() || !ob->IsInteractabe()) continue;

			Vector2 p = ob->objectTransform.position;
			if ((int)floorf(p.x / tileSize) == cx && (int)floorf(p.y / tileSize) == cy)
				return ob.get();
		}
		return nullptr;
	}
};