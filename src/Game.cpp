#include "Game.h"
#include <raylib.h>

Game::Game(int width, int height, const char* title)
{
	InitWindow(width, height, title);
	InitAudioDevice();
	SetTargetFPS(60);
}

Game::~Game()
{
	current_scene.reset(); // Releases the scene while the window is still open
	CloseWindow();
	CloseAudioDevice();
}

void Game::SetScene(std::unique_ptr<Scene> scene)
{
	current_scene = std::move(scene);
}

bool LogInitSceneError() {
	TraceLog(LOG_ERROR, "Scene failed to start");
	return false;
}

bool start = false;

bool Game::Run() {
	// -- Start --
	if (!current_scene->Start()) {
		return LogInitSceneError();
	}

	while (current_scene && !WindowShouldClose()) {
		
		// -- Update --
		if (start)
			current_scene->Update(GetFrameTime());

		// -- Render --
		BeginDrawing();

		ClearBackground(BLACK);
		current_scene->Render();

		EndDrawing();

		if (!start) {
			start = IsKeyDown(KEY_P);
			continue;
		}

		// -- Scene change --
		auto next_scene = current_scene->NextScene();
		if (next_scene != nullptr) {
			current_scene = std::move(next_scene);
			// Initialize the next scene
			if (!current_scene->Start()) {
				return LogInitSceneError();
			}
		}
	}

	return true;
}