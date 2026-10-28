#pragma once
#include "Scene.h"

#include "Levels.h"
#include "WinScene.h"
#include "GameOverScene.h"
#include "Object.h"
#include "ObjectControllers.h"
#include "MovableObjects.h"
#include "InteractiveOverlay.h"
#include "Grid.h"
#include "ObjectHandler.h"
#include "Assets.h"
#include "CameraUtils.h"
#include "MenuUI.h"
#include "SoundManager.h"
#include "TurnManager.h"

#include <raylib.h>
#include <vector>
#include <functional>
#include <memory>

using SceneFactory = std::function<std::unique_ptr<Scene>()>;

class LevelScene : public Scene
{
public:
	explicit LevelScene(int level_index) : level_index(level_index) {}

	bool Start() override {
		// Load the tileset
		if (!assets.Load("assets/sprites/tilemap.png")) return false;
		// Load all the sounds
		soundManager.InitSounds();
			// Play the background loop
		soundManager.PlayStoredSound(SoundManager::SoundType::BG_MUSIC);

		soundManager.PlayStoredSound(SoundManager::SoundType::START_LEVEL);

		overlay.SetSoundManager(&soundManager);

		// Load the map
		grid.SetSize(64);
		if (!grid.ScanLevel(LEVELS_INFO[level_index].fileName)) return false;

		turnManager.SetGrid(&grid);

		std::vector<Grid::ObjectPosition> positions = grid.GetStartPositions();

		SpawnObjectsFromPositions(positions);

		objectHandler.EndTurn();

		auto& objects = objectHandler.Objects();

		// Put the player object at the start of the vector
		if (player_ptr != nullptr) {
			auto it = std::find_if(objects.begin(), objects.end(),
				[this](const std::unique_ptr<Object>& ob) {
					return ob.get() == player_ptr;
				}
			);
			if (it != objects.begin()) {
				std::rotate(objects.begin(), it, it + 1);
			}
		}
		else {
			TraceLog(LOG_ERROR, "The player hasn't been registered. The game can't be completed.");
		}

		RestartScene(positions);

		InitUI();

		return true;
	}

	void Update(float deltaTime) override {
		menuUI.button.Update();

		soundManager.UpdateSounds();

		auto& objects = objectHandler.Objects();

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);

			soundManager.PlayStoredSound(SoundManager::SoundType::UI_ERROR);

			if (!overlay.IsVisible()) {
				Object* interacted = FindInteractableAtPos(mouse_pos);
				if (interacted != nullptr) {
					interacted_id = interacted->GetId();
					Vector2 pos = interacted->objectTransform.position;
					overlay.SetAnchor(Vector2{ pos.x, pos.y + grid.tile_size });
					overlay.SetVisible(true);
				}
			}
			else {
				int idx = overlay.GetClickedOption(mouse_pos, grid);
				Action ac = overlay.GetIndexAction(idx);
				Object* ob = Interacted();
				if (ob != nullptr) ob->ExecuteAction(ac, objects);
				interacted_id = -1;
				overlay.SetVisible(false);
			}
		}
		
		if (player_ptr)
			turnManager.Update(objectHandler, *player_ptr, IsMouseButtonPressed(MOUSE_BUTTON_RIGHT));

		for (auto& ob : objectHandler.Objects())
			ob->UpdateMovement(deltaTime);
		objectHandler.EndTurn();
	}

	void Render() override {
		DrawFPS(0, 0);
		camera = MakeFitCamera(grid.GetBounds());
		
		BeginMode2D(camera);
		grid.DrawGrid(assets);
		for (auto& ob : objectHandler.Objects()) {
			ob->DrawObject(assets);
		}

		overlay.DrawOverlay(assets, grid, Interacted());
		
		EndMode2D();

		menuUI.Render(assets);
	}

	std::unique_ptr<Scene> NextScene() override {
		if (!next_scene_factory) return nullptr;
		return next_scene_factory();
	}

	void RestartScene(std::vector<Grid::ObjectPosition> positions) {
		// Respawn all the dead objects
		std::vector<Grid::ObjectPosition> to_spawn;
		std::vector<std::unique_ptr<Object>>& objects = objectHandler.Objects();
		for (int i = 0; i < positions.size(); i++) {
			bool alive = false;
			for (auto& ob : objects)
				if (positions[i].type == ob->type) alive = true;

			if (!alive) to_spawn.push_back(positions[i]);
		}
		SpawnObjectsFromPositions(to_spawn);
		objectHandler.EndTurn();

		// Reposition all the objects
		for (int i = 0; i < objects.size(); i++) {
			objects[i]->SetSize(grid.tile_size);
			for (int j = 0; j < positions.size(); j++) {
				if (positions[j].type == objects[i]->type) {
					// Restore the avaliable moves on interactable objects
					if (objects[i]->IsInteractabe())
						objects[i]->SetMoves(Object::AVALIABLE_MOVES);
					if (objects[i]->type == EntityType::DOOR) {
						objects[i]->Interact(0);
					}

					objects[i]->SetLife(objects[i]->GetMaxLife());

					objects[i]->AnimateRotation(90, 1, Easing::OutBack);
					objects[i]->AnimateTo(positions[j].position * grid.tile_size, 1, Easing::OutBack);
					// Deassign the position type so other objects can assign it to themselves
					positions[j].type = EntityType::NONE;
					break;
				}
			}
		}
	}
private:
	bool changeScene = false;
	std::function<std::unique_ptr<Scene>()> next_scene_factory;
	void SetNextScene(SceneFactory sceneFactory) {
		next_scene_factory = std::move(sceneFactory);
	}
	void GoToNextLevelOrWin() {
		const int next = level_index++;
		if (next < LEVEL_COUNT)
			SetNextScene([next] { return std::make_unique<LevelScene>(next); });
		else
			SetNextScene([next] { return std::make_unique<WinScene>(); });
	}
	
	Grid grid;
	ObjectHandler objectHandler;

	PlayerController* player_ptr = nullptr;
	int interacted_id = -1;
	Object* Interacted() { return interacted_id < 0 ? nullptr : FindById(interacted_id); };

	Assets assets;

	Camera2D camera{};
	InteractiveOverlay overlay;
	
	SoundManager soundManager;

	TurnManager turnManager;

	MenuUI menuUI;

	int level_index = 0;

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

	void InitUI() {
		menuUI.button.on_click = [this]() { 
			objectHandler.RemoveType(EntityType::BULLET);
			soundManager.PlayStoredSound(SoundManager::SoundType::REWIND);
			RestartScene(grid.GetStartPositions());
		};
	}

	int enemies_alive = 0;
	void OnEnemyKilled() {
		// If the level goal is to kill all the enemies it will open the door when they are killed
		if (--enemies_alive > 0) return;
		if (LEVELS_INFO[level_index].goal == LevelGoal::KILL_ENEMIES) {
			objectHandler.InteractWithObject(EntityType::DOOR);
		}
	}

	void SpawnObjectsFromPositions(std::vector<Grid::ObjectPosition>& positions) {
		for (int i = 0; i < positions.size(); i++) {
			Object* ob = nullptr;
			switch (positions[i].type) {
			case EntityType::PLAYER:
			{
				auto& player = objectHandler.Spawn<PlayerController>(&grid, GREEN, 3);
				player_ptr = &player; // Store the player ptr
				player.on_die = [this]() {
					SetNextScene([] {
						return std::make_unique<GameOverScene>();
						});
					changeScene = true;
					player_ptr = nullptr;
					};
				ob = &player;
				break;
			}
			case EntityType::ENEMY:
			{
				auto& enemy = objectHandler.Spawn<EnemyController>(&grid, RED, 3);
				enemy.on_die = [this]() { OnEnemyKilled(); };
				enemies_alive++;
				ob = &enemy;
				break;
			}
			case EntityType::CANNON:
			{
				auto& cannon = objectHandler.Spawn<CannonController>(&grid, YELLOW, 1);
				cannon.SetInteractable(true);
				cannon.on_shoot = [this]() { soundManager.PlayStoredSound(SoundManager::SHOOT); };
				ob = &cannon;
				break;
			}
			case EntityType::CRATE:
			{
				auto& crate = objectHandler.Spawn<CrateController>(&grid, BROWN, 1);
				crate.SetInteractable(true);
				crate.on_hit = [this]() { soundManager.PlayStoredSound(SoundManager::SoundType::CRATE_HIT); };
				ob = &crate;
				break;
			}
			case EntityType::TARGET:
			{
				auto& target = objectHandler.Spawn<TargetController>(&grid, RED, 1);
				target.on_hit = [this]() { objectHandler.InteractWithObject(EntityType::DOOR); };
				ob = &target;
				break;
			}
			case EntityType::DOOR:
			{
				auto& door = objectHandler.Spawn<DoorController>(&grid, BROWN, 1);
				door.on_hit = [this]() { changeScene = true; GoToNextLevelOrWin(); };
				ob = &door;
				break;
			}
			}

			if (ob != nullptr) {
				if (!ob->on_move) ob->on_move = [this]() { soundManager.PlayStoredSound(SoundManager::SoundType::MOVE); };
				if (!ob->on_rotate) ob->on_rotate = [this]() { soundManager.PlayStoredSound(SoundManager::SoundType::ROTATE); };
				if (!ob->on_hit) ob->on_hit = [this]() { soundManager.PlayStoredSound(SoundManager::SoundType::HIT); };

				if (ob->IsInteractabe()) ob->SetMoves(9);
			}
		}
	}
};