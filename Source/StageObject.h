#pragma once

#include "Lib/Lib.h"

class StageObject
{
public:

	StageObject() {}
	virtual ~StageObject() {}

	// 更新処理
	virtual void Update(float elapsedTime) = 0;

	// 描画処理
	virtual void Render() = 0;

	virtual void RenderSub() = 0;

	virtual void DebugRender() = 0;

protected:

	PP::WorldContext wc = {};

	float radius = 0.0f;
	float height = 0.0f;

};

