#include "WinScene.h"
#include "Level1.h"

std::unique_ptr<Scene> WinScene::NextScene()
{
	return change_scene ? std::make_unique<Level1>() : nullptr;
}