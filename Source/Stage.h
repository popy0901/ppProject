#pragma once

#include "Lib/Lib.h"

class Stage
{
public:

	Stage() {}
	virtual ~Stage() {}

	// �X�V����
	virtual void Update(float elapsedTime) = 0;

	// �`�揈��
	virtual void Render() = 0;

	virtual void RenderSub() = 0;

	virtual void DebugRender() = 0;

	// ���C�L���X�g
	virtual bool RayCast(
		const DirectX::XMFLOAT3& start,
		const DirectX::XMFLOAT3& end,
		PP::HitResult& hit) = 0;
};

