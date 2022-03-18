#pragma once
#include <DirectXMath.h>


namespace PP
{
	struct WorldContext // オイラー用
	{
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT3 angle;
		DirectX::XMFLOAT3 position;
	};

	struct WorldContextQ // クォータニオン用
	{
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT4 quaternion;
		DirectX::XMFLOAT3 position;
	};
}