#pragma once

#include <DirectXMath.h>

namespace PP
{
	// �����_�[�R���e�L�X�g
	struct RenderContext
	{
		DirectX::XMFLOAT4X4		view;
		DirectX::XMFLOAT4X4		projection;
		//DirectX::XMFLOAT4		lightDirection;
	};

}