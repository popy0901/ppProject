#include "LibCamera.h"



namespace PP
{
	namespace Class
	{
		void LibCamera::SetLookAt(
			const DirectX::XMFLOAT3& pos,
			const DirectX::XMFLOAT3& target,
			const DirectX::XMFLOAT3& up)
		{
			DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&pos);
			DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&target);
			DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);

			DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
			DirectX::XMStoreFloat4x4(&view, View);

			// ビュー行列を逆行列化(ワールド行列に変換)
			DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
			DirectX::XMFLOAT4X4 world;
			DirectX::XMStoreFloat4x4(&world, World);

			// カメラの方向を取り出す
			this->right = { world._11, world._12, world._13 };
			this->up = { world._21, world._22, world._23 };
			this->front = { world._31, world._32, world._33 };

			this->pos = pos;
			this->target = target;

		}

		void LibCamera::SetPerspective(float fovY, float aspect, float nearZ, float farZ)
		{
			DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
			DirectX::XMStoreFloat4x4(&projection, Projection); // プロジェクション行列作成

		}
	}
}