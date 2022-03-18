#pragma once
#include <DirectXMath.h>


namespace PP
{
	struct WorldContext // �I�C���[�p
	{
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT3 angle;
		DirectX::XMFLOAT3 position;
	};

	struct WorldContextQ // �N�H�[�^�j�I���p
	{
		DirectX::XMFLOAT3 scale;
		DirectX::XMFLOAT4 quaternion;
		DirectX::XMFLOAT3 position;
	};
}