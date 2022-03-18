#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

namespace PP
{
	namespace Class
	{
		class LineRenderer
		{
		public:
			LineRenderer(ID3D11Device* device, UINT vertexCount);
			~LineRenderer() {}

		public:
			// ï`âÊé¿çs
			void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

			// í∏ì_í«â¡
			void AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color);

			// ê¸ï`âÊ
			void DrawLine(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2, const DirectX::XMFLOAT4& color);

			void DrawCube(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2, const DirectX::XMFLOAT4& color);

		private:
			struct ConstantBuffer
			{
				DirectX::XMFLOAT4X4	wvp;
			};

			struct Vertex
			{
				DirectX::XMFLOAT3	position;
				DirectX::XMFLOAT4	color;
			};


			Microsoft::WRL::ComPtr<ID3D11Buffer>			vertexBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer = nullptr;

			Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout = nullptr;

			Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState = nullptr;

			std::vector<Vertex>			vertices;
			UINT						capacity = 0;
		};

	}
}