#pragma once

#include "Scene.h"

class SceneClear : public Scene
{
public:

	SceneClear() {}
	~SceneClear() override {}

	void Init() override;

	void Update(float elapsedTime) override;

	void Render() override;

	void DebugRender() override;

	void End() override;

private:



};