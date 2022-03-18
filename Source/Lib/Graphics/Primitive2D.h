#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

namespace PP
{
	namespace Class
	{
		class Primitive2d
		{
		public:

			Primitive2d(ID3D11Device* device);
			~Primitive2d() {}

			struct Vertex
			{
				DirectX::XMFLOAT3 position;
				DirectX::XMFLOAT4 color;
			};

			void Rect(ID3D11DeviceContext* dc,
				const DirectX::XMFLOAT2& pos, const DirectX::XMFLOAT2& size,
				const DirectX::XMFLOAT2& center, float angle,
				const DirectX::XMFLOAT4& color);

			void Circle(ID3D11DeviceContext* dc,
				const DirectX::XMFLOAT2& pos, float radius,
				const DirectX::XMFLOAT4& color, int n);

			void Line(ID3D11DeviceContext* dc,
				const DirectX::XMFLOAT2& pos1, const DirectX::XMFLOAT2& pos2,
				const DirectX::XMFLOAT4& color, int width);

		private:

			Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout = nullptr;

			Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer = nullptr;

			Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState = nullptr;

			static const int PRIMITIVE_VERTEX_NUM = 128;

		};
	}
}
