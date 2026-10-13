#pragma once
#include "Scene.h"
#include "Level2.h"
#include <raylib.h>

class Level1 : public Scene
{
public:

	void Update(float deltaTime) override {
		start = IsKeyPressed(KEY_SPACE);
	}

	void Render() override {
		DrawText("Level 1 Scene", 250, 250, 10, GREEN);
	}

	std::unique_ptr<Scene> NextScene() override {
		return start ? std::make_unique<Level2>() : nullptr;
	}
private:
	bool start = false;
};