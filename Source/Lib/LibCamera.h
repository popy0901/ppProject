#pragma once

#include <DirectXMath.h>

namespace PP
{
	namespace Class
	{
		class LibCamera
		{
		public:

			LibCamera() {}
			~LibCamera() {}

			void SetLookAt(
				const DirectX::XMFLOAT3& pos,
				const DirectX::XMFLOAT3& target,
				const DirectX::XMFLOAT3& up);

			void SetPerspective(float fovY, float aspect, float nearZ, float farZ);

			const DirectX::XMFLOAT4X4& GetView() const { return view; }

			const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

			const DirectX::XMFLOAT3& GetPos() const { return pos; }

			const DirectX::XMFLOAT3& GetTarget() const { return target; }

			const DirectX::XMFLOAT3& GetRight() const { return right; }

			const DirectX::XMFLOAT3& GetUp() const { return up; }

			const DirectX::XMFLOAT3& GetFront() const { return front; }

		private:

			DirectX::XMFLOAT4X4 view = {};
			DirectX::XMFLOAT4X4 projection = {};

			DirectX::XMFLOAT3 pos = {}; // eye
			DirectX::XMFLOAT3 target = {}; // focus

			DirectX::XMFLOAT3 right = {};
			DirectX::XMFLOAT3 up = {};
			DirectX::XMFLOAT3 front = {};

		};
	}
}