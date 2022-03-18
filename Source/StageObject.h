#pragma once

#include "Lib/Lib.h"

class StageObject
{
public:

	StageObject() {}
	virtual ~StageObject() {}

	// �X�V����
	virtual void Update(float elapsedTime) = 0;

	// �`�揈��
	virtual void Render() = 0;

	virtual void RenderSub() = 0;

	virtual void DebugRender() = 0;

protected:

	PP::WorldContext wc = {};

	float radius = 0.0f;
	float height = 0.0f;

};

