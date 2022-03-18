#pragma once

#include "Lib/Lib.h"

class Stage
{
public:

	Stage() {}
	virtual ~Stage() {}

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render() = 0;

	virtual void RenderSub() = 0;

	virtual void DebugRender() = 0;

	// レイキャスト
	virtual bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		PP::HitResult& hit) = 0;
};

