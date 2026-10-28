#include "GameOverScene.h"
#include "LevelScene.h"

std::unique_ptr<Scene> GameOverScene::NextScene()
{
	return change_scene ? std::make_unique<LevelScene>(0) : nullptr;
}