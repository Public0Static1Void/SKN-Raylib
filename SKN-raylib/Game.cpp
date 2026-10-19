#include "Game.h"
#include <raylib.h>

Game::Game(int width, int height, const char* title)
{
	InitWindow(width, height, title);
	SetTargetFPS(60);
}

Game::~Game()
{
	current_scene.reset(); // Releases the scene while the window is still open
	CloseWindow();
}

void Game::SetScene(std::unique_ptr<Scene> scene)
{
	current_scene = std::move(scene);
}

void Game::Run() {
	// -- Start --
	current_scene->Start();

	while (current_scene && !WindowShouldClose()) {
		// -- Update --
		current_scene->Update(GetFrameTime());

		// -- Render --
		BeginDrawing();

		ClearBackground(BLACK);
		current_scene->Render();

		EndDrawing();

		// -- Scene change --
		auto next_scene = current_scene->NextScene();
		if (next_scene != nullptr) {
			current_scene = std::move(next_scene);
			current_scene->Start();
		}
			
	}
}