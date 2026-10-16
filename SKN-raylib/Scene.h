#pragma once
#include <memory>

class Scene
{
public:
	virtual ~Scene() = default;

	virtual void Start();
	virtual void Update(float deltaTime);
	virtual void Render();

	// Returns a new scene to change to if overwrited
	virtual std::unique_ptr<Scene> NextScene() { return nullptr; }
};