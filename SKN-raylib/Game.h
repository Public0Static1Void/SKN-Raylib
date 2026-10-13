#pragma once
#include "Scene.h"

struct GameState {

};

class Game
{
public:
	Game(int width, int height, const char* name);
	~Game();

	Game(const Game&) = delete;
	Game& operator= (const Game&) = delete;

	void SetScene(std::unique_ptr<Scene> scene);
	void Run();

private:
	std::unique_ptr<Scene> current_scene;
};