#pragma once

#include "Stage.h"

// ステージマネージャー
class StageManager
{
private:

	StageManager() {}
	~StageManager() {}

public:

	static StageManager& Instance()
	{
		static StageManager instance;
		return instance;
	};

	void Init();

	void Update(float elapsedTime);

	void Render();

	void RenderSub();

	void DebugRender();

	void Register(Stage* stage);

	void Clear();

	bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		PP::HitResult& hit);

private:

	std::vector<Stage*> stages;

};