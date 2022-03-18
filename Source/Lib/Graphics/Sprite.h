#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>

namespace PP
{
	namespace Class
	{
		// スプライト
		class Sprite
		{
		public:
			Sprite();
			Sprite(const char* filename);
			~Sprite() {}

			struct Vertex
			{
				DirectX::XMFLOAT3	position;
				DirectX::XMFLOAT4	color;
				DirectX::XMFLOAT2	texcoord;
			};

			// 描画実行
			void Render(ID3D11DeviceContext* dc,
				float dx, float dy,
				float dw, float dh,
				float sx, float sy,
				float sw, float sh,
				float angle,
				float r, float g, float b, float a) const;

			// テクスチャ幅取得
			int GetTextureWidth() const { return textureWidth; }

			// テクスチャ高さ取得
			int GetTextureHeight() const { return textureHeight; }

		private:
			Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader = nullptr;
			Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout = nullptr;

			Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer = nullptr;

			Microsoft::WRL::ComPtr<ID3D11BlendState>			blendState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState>		rasterizerState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilState>		depthStencilState = nullptr;

			Microsoft::WRL::ComPtr<ID3D11SamplerState>			samplerState = nullptr;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView = nullptr;

			int textureWidth = 0;
			int textureHeight = 0;
		};
	}
}