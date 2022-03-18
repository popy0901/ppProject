#pragma once

#include <wrl.h>
#include <memory>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Lib/Graphics/RenderContext.h"

namespace PP
{
	namespace Class
	{
		class LibParticle
		{
		public:

			LibParticle() {}
			LibParticle(const char* fileName, int num = 10000);
			~LibParticle() { delete[] data; delete[] vertex; }

			void Update(float elapsedTime);

			void Render(ID3D11DeviceContext* dc, const RenderContext& rc);

			void Animation(float elapsedTime, float speed = 0.1f);

			void Set(int type, float timer,
				DirectX::XMFLOAT3 p,
				DirectX::XMFLOAT3 v = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
				DirectX::XMFLOAT3 f = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
				DirectX::XMFLOAT2 size = DirectX::XMFLOAT2(1.0f, 1.0f));

			void Snow(DirectX::XMFLOAT3 pos, int max);

			void Spark(DirectX::XMFLOAT3 pos, int max);
			void Fire(DirectX::XMFLOAT3 pos, int max);
			void Smoke(DirectX::XMFLOAT3 pos, int max);

		private:

			struct Data
			{
				float x, y, z;
				float w, h;
				float aw, ah;
				float vx, vy, vz;
				float ax, ay, az;
				float alpha;
				float timer;
				float anime_timer;
				float scale;
				float angle;
				int type;
			};

			struct Vertex
			{
				DirectX::XMFLOAT3	Position;
				DirectX::XMFLOAT3	Normal; // type, angle, 
				DirectX::XMFLOAT2	Tex;
				DirectX::XMFLOAT4	Color;
			};

			Data*	data = nullptr;
			Vertex* vertex = nullptr;

			int num = 0;

			// 定数バッファのデータ定義
			struct ConstantBufferForPerFrame 
			{
				DirectX::XMFLOAT4X4	view;			// ビュー変換行列
				DirectX::XMFLOAT4X4	projection;		// 透視変換行列
			};

			Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer = nullptr;
			Microsoft::WRL::ComPtr<ID3D11Buffer>				constantBuffer = nullptr;

			Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11GeometryShader>		geometryShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout = nullptr;

			Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState[2] = {};

			Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView = nullptr;

		};
	}
}