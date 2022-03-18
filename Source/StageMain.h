#pragma once

#include "Stage.h"
#include "Navigation.h"

// ステージ
class StageMain : public Stage
{
public:

	StageMain();
	~StageMain() override;

	void Update(float elapsedTime) override;

	void Render() override;

	void RenderSub() override;

	void DebugRender() override;

	// レイキャスト
	bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		PP::HitResult& hit) override;

private:

	PP::ppModel model[3] = {};

	Navigation* navi = nullptr;

};