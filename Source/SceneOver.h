#pragma once

#include "Scene.h"

class SceneOver : public Scene
{
public:

	SceneOver() {}
	~SceneOver() override {}

	void Init() override;

	void Update(float elapsedTime) override;

	void Render() override;

	void DebugRender() override;

	void End() override;

private:



};