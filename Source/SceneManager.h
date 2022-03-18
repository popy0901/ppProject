#pragma once

#include <memory>
#include "Scene.h"

class SceneManager
{
public:

	SceneManager() {}
	~SceneManager() {}

	void Update(float elapsedTime);
	
	void Render();

	void DebugRender();
	
	void End();
	
	void ChangeScene(Scene* scene);

	void SetChangeType(const int changeType) { this->changeType = changeType; }

	int GetChangeType() { return changeType; }

	static SceneManager* Instance()
	{
		static SceneManager instance;
		return &instance;
	}

	enum ClearOver
	{
		NONE = 0,
		CLEAR = 1,
		OVER = 2,
	};

private:

	std::unique_ptr<Scene> currentScene = nullptr;

	int changeType = 0;

};