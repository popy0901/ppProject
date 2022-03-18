#pragma once

#include <DirectXMath.h>

namespace PP
{
	// レンダーコンテキスト
	struct RenderContext
	{
		DirectX::XMFLOAT4X4		view;
		DirectX::XMFLOAT4X4		projection;
		//DirectX::XMFLOAT4		lightDirection;
	};

}