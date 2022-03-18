#pragma once

#include "Scene.h"

class SceneLoad : public Scene
{
public:

	SceneLoad(Scene* nextScene) : nextScene(nextScene) {}
	~SceneLoad() override {}

	void Init() override;
	
	void Update(float elapsedTime) override;
	
	void Render() override;
	
	void DebugRender() override;
	
	void End() override;

private:

	static void LoadingThread(SceneLoad* scene);

	Scene* nextScene = nullptr;

};