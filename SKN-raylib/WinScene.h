#pragma once
#pragma once
#include <raylib.h>
#include "Scene.h"

class WinScene : public Scene {
public:
	void Render() override {
		DrawText("You win, congrats!\nPress any key to repeat the level.", 250, 250, 40, GREEN);
	}

	void Update(float deltaTime) override {
		change_scene = GetKeyPressed() != 0;
	}

	std::unique_ptr<Scene> NextScene() override;

private:
	bool change_scene = false;
};