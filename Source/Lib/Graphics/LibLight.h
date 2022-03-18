#pragma once
#include <DirectXMath.h>


namespace PP
{
	namespace Class
	{
		class LibLight
		{
		public:

			LibLight() {}
			~LibLight() {}

			void SetPosition(DirectX::XMFLOAT3 position);

			void SetDirection(DirectX::XMFLOAT4 direction);

			void SetColor(DirectX::XMFLOAT4 color);

			void SetAmbient(DirectX::XMFLOAT4 ambient);

			const DirectX::XMFLOAT3& GetPosition() const { return position; }

			const DirectX::XMFLOAT4& GetDirection() const { return direction; }

			const DirectX::XMFLOAT4& GetColor() const { return color; }

			const DirectX::XMFLOAT4& GetAmbient() const { return ambient; }

			// 後々スポットライト、ポイントライトを追加する

		private:

			DirectX::XMFLOAT3 position = { 0, 0, 0 };
			DirectX::XMFLOAT4 direction = { 1, 1, 1, 1 };
			DirectX::XMFLOAT4 color = { 1, 1, 1, 1 };
			DirectX::XMFLOAT4 ambient = { 1, 1, 1, 1 };

		};
	}
}