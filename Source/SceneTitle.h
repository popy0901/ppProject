#pragma once

#include "Scene.h"

class SceneTitle : public Scene
{
public:

	SceneTitle() {}
	~SceneTitle() override {}

	void Init() override;

	void Update(float elapsedTime) override;

	void Render() override;
	
	void DebugRender() override;

	void End() override;

private:



};