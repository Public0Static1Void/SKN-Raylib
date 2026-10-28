#include "WinScene.h"
#include "LevelScene.h"

std::unique_ptr<Scene> WinScene::NextScene()
{
	return change_scene ? std::make_unique<LevelScene>(0) : nullptr;
}