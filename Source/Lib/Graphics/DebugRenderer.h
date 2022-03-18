#pragma once

#include <vector>
#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

namespace PP
{
	namespace Class
	{
		class DebugRenderer
		{
		public:
			DebugRenderer(ID3D11Device* device);
			~DebugRenderer() {}

		public:
			// ï`âÊé¿çs
			void Render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

			// ãÖï`âÊ
			void DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT4& color);

			// â~íåï`âÊ
			void DrawCylinder(const DirectX::XMFLOAT3& position, float radius, float height, const DirectX::XMFLOAT4& color);

		private:
			// ãÖÉÅÉbÉVÉÖçÏê¨
			void CreateSphereMesh(ID3D11Device* device, float radius, int slices, int stacks);

			// â~íåÉÅÉbÉVÉÖçÏê¨
			void CreateCylinderMesh(ID3D11Device* device, float radius1, float radius2, float start, float height, int slices, int stacks);

		private:
			struct CbMesh
			{
				DirectX::XMFLOAT4X4	wvp;
				DirectX::XMFLOAT4	color;
			};

			struct Sphere
			{
				DirectX::XMFLOAT4	color;
				DirectX::XMFLOAT3	center;
				float				radius;
			};

			struct Cylinder
			{
				DirectX::XMFLOAT4	color;
				DirectX::XMFLOAT3	position;
				float				radius;
				float				height;
			};

			Microsoft::WRL::ComPtr<ID3D11Buffer>			sphereVertexBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Buffer>			cylinderVertexBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Buffer>			constantBuffer = nullptr;

			Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout = nullptr;

			Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState = nullptr;

			std::vector<Sphere>		spheres;
			std::vector<Cylinder>	cylinders;

			UINT	sphereVertexCount = 0;
			UINT	cylinderVertexCount = 0;
		};
	}
}
