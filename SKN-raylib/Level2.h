#pragma once
#include <raylib.h>
#include "Scene.h"
class Level2 : public Scene
{
	void Render() override {
		DrawText("Level 2 Scene", 250, 250, 10, GREEN);
	}
};