#pragma once

#include "Scene.h"
#include "MetaAI.h"
#include "Sky.h"
#include "Camera.h"
#include "UiManager.h"
#include "StageObjectManager.h"

class SceneGame : public Scene
{
public:

	SceneGame() {}
	~SceneGame() override {}

	void Init() override;
	
	void Update(float elapsedTime) override;
	
	void Render() override;
	
	void DebugRender() override;
	
	void End() override;

private:

	Sky* sky = nullptr;

	Camera* camera = nullptr;

	UiManager* uiManager = nullptr;

	StageObjectManager* stageObjectManager = nullptr;
	
	Meta* meta = nullptr;

	DirectX::XMFLOAT3 lightPosition = { 0, 30, 0 };
	DirectX::XMFLOAT4 lightDirection = { 1.0f, -1.0f, 1.0f, 1.0f };
};