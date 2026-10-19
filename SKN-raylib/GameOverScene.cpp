#include "GameOverScene.h"
#include "Level1.h"

std::unique_ptr<Scene> GameOverScene::NextScene()
{
	return change_scene ? std::make_unique<Level1>() : nullptr;
}